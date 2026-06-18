#include "costcat/auth.hpp"
#include "jdbc/cppconn/exception.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/statement.h"
#include <api/api.hpp>
#include <api/sql.hpp>
#include <chrono>
#include <costcat/auth.hpp>
#include <costcat/costcat.hpp>
#include <exception>
#include <httplib.h>
#include <iostream>
#include <memory>

#define AUTHCAT_CLIENT_MODE
#include <AuthCat/auth.hpp>

#include <nlohmann/json.hpp>

using namespace nathcat::cost;
using json = nlohmann::json;

void nathcat::cost::log_transaction(const httplib::Request &req,
                                    httplib::Response &res) {
  // Check authentication
  std::cout << "> Log transaction" << std::endl;

  nathcat::auth::User user;
  try {
    user = authenticate_request(req, res);
  } catch (std::exception &e) {
    return;
  }

  std::cout << "\tPassed authentication." << std::endl;

  // Check that the group parameter has been specified
  if (!req.has_param("group")) {
    std::cout << "\tMissing group param." << std::endl;

    res.status = httplib::StatusCode::BadRequest_400;
    res.set_content("Missing group param!", "text/plain");
    return;
  }

  // Check that the authenticated user is a member of the specified group
  int group = std::stoi(req.get_param_value("group"));

  try {
    std::unique_ptr<sql::Connection> db{
        sqlDriver->connect(config.dbUrl, config.dbUsername, config.dbPassword)};
    db->setSchema("CostCat");

    if (!sqlwrapper::util::isMemberOfGroup(db, user.id, group)) {
      std::cout
          << "\tAuthenticated user is not a member of the specified group."
          << std::endl;

      res.status = httplib::StatusCode::Forbidden_403;
      res.set_content("You are not a member of the specified group",
                      "text/plain");
      return;
    }

    // Get the request body and insert it into the DB
    try {
      json body = json::parse(req.body);

      struct transaction_request t = body.get<struct transaction_request>();

      nathcat::sqlwrapper::start_transaction(db);

      // Insert transaction
      std::unique_ptr<sql::PreparedStatement> pStmt{db->prepareStatement(
          "INSERT INTO `Transactions` (`payer`, `amount`, "
          "`payeeCount`, `group`, `timestamp`, "
          "`description`) VALUES (?, ?, ?, ?, unix_timestamp(), ?)")};
      pStmt->setInt(1, user.id);
      pStmt->setInt(2, t.amount);
      pStmt->setInt(3, t.payees.size());
      pStmt->setInt(4, group);
      pStmt->setString(5, t.description);

      pStmt->executeUpdate();
      pStmt->close();

      // Get auto generated key
      std::unique_ptr<sql::Statement> stmt{db->createStatement()};
      std::unique_ptr<sql::ResultSet> rs{
          stmt->executeQuery("SELECT LAST_INSERT_ID() AS 'id'")};

      rs->next();
      int transactionId = rs->getInt("id");
      stmt->close();

      // Insert payees
      for (int i = 0; i < t.payees.size(); i++) {
        pStmt = std::unique_ptr<sql::PreparedStatement>{db->prepareStatement(
            "INSERT INTO Payees (`transaction`, `user`) VALUES (?, ?)")};

        pStmt->setInt(1, transactionId);
        pStmt->setInt(2, t.payees[i]);
        pStmt->executeUpdate();
        pStmt->close();
      }

      // Update running totals
      // Do the payer first
      pStmt = std::unique_ptr<sql::PreparedStatement>{db->prepareStatement(
          "INSERT INTO RunningTotals (`user`, `group`, `balance`) values (?, "
          "?, ?) on duplicate key update `balance` = `balance` + values "
          "(`balance`)")};

      pStmt->setInt(1, user.id);
      pStmt->setInt(2, group);
      pStmt->setInt(3, t.amount);
      pStmt->executeUpdate();
      pStmt->close();

      // And now the payees (who are now in debt to the payer)
      for (int i = 0; i < t.payees.size(); i++) {
        pStmt = std::unique_ptr<sql::PreparedStatement>{db->prepareStatement(
            "INSERT INTO RunningTotals (`user`, `group`, `balance`) values (?, "
            "?, ?) on duplicate key update `balance` = `balance` + values "
            "(`balance`)")};
        pStmt->setInt(1, t.payees[i]);
        pStmt->setInt(2, group);
        pStmt->setInt(3, -t.amount);
        pStmt->executeUpdate();
        pStmt->close();
      }

      success_response(res);

      nathcat::sqlwrapper::commit_transaction(db);

    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;

      res.status = httplib::StatusCode::InternalServerError_500;
      res.set_content(e.what(), "text/plain");

      nathcat::sqlwrapper::rollback_transaction(db);
      db->close();
      return;
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;

    res.status = httplib::StatusCode::InternalServerError_500;
    res.set_content(e.what(), "text/plain");
    return;
  }
}
