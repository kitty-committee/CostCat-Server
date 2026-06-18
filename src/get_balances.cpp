#include "httplib.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"
#include "nlohmann/json_fwd.hpp"
#include <api/sql.hpp>
#include <costcat/auth.hpp>
#include <costcat/costcat.hpp>
using namespace nathcat;

void nathcat::cost::get_balances(const httplib::Request &req,
                                 httplib::Response &res) {
  std::cout << "> Get balances" << std::endl;

  nathcat::auth::User user;
  try {
    user = cost::authenticate_request(req, res);
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
    std::unique_ptr<sql::Connection> db{cost::sqlDriver->connect(
        cost::config.dbUrl, cost::config.dbUsername, cost::config.dbPassword)};
    db->setSchema("CostCat");

    if (!sqlwrapper::util::isMemberOfGroup(db, user.id, group)) {
      std::cout
          << "\tAuthenticated user is not a member of the specified group."
          << std::endl;
      db->close();

      res.status = httplib::StatusCode::Forbidden_403;
      res.set_content("You are not a member of the specified group",
                      "text/plain");
      return;
    }

    // Get the balances for the group
    std::unique_ptr<sql::PreparedStatement> pStmt{
        db->prepareStatement("select * from RunningTotals where `group` = ?")};
    pStmt->setInt(1, group);

    std::unique_ptr<sql::ResultSet> rs{pStmt->executeQuery()};

    // Pass the results into the body of the response
    std::vector<struct cost::balance> balances =
        sqlwrapper::toArray<struct cost::balance>(rs);

    nlohmann::json body(balances);

    res.status = httplib::StatusCode::OK_200;
    res.set_content(body.dump(), "application/json");
    return;

  } catch (std::exception &e) {
    res.status = httplib::StatusCode::InternalServerError_500;
    res.set_content(e.what(), "text/plain");
    return;
  }
}
