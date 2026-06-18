#include "httplib.h"
#include <api/sql.hpp>
#include <costcat/auth.hpp>
#include <costcat/costcat.hpp>
using namespace nathcat;

void nathcat::cost::get_debts(const httplib::Request &req,
                              httplib::Response &res) {

  // Check authentication
  std::cout << "> Get debts" << std::endl;

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

    std::vector<struct cost::balance> balances =
        cost::util::get_balances(db, group);

    std::vector<std::vector<struct cost::balance>> subsets =
        zero_subset_sum(balances);

    std::vector<struct debt> debts;

    for (int i = 0; i < subsets.size(); i++) {
      int collector = subsets[i][0].user;
      std::vector<struct debt> subset_debts =
          simplify_with_collector(collector, subsets[i]);

      for (int j = 0; j < subset_debts.size(); j++) {
        debts.push_back(subset_debts[j]);
      }
    }

    // Pass the debts into a JSON body
    nlohmann::json body(debts);

    res.status = httplib::StatusCode::OK_200;
    res.set_content(body.dump(), "application/json");
    return;

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;

    res.status = httplib::StatusCode::InternalServerError_500;
    res.set_content(e.what(), "text/plain");
    return;
  }
}
