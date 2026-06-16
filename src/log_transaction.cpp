#include <api/sql.hpp>
#include <costcat/costcat.hpp>
#include <httplib.h>
using namespace nathcat::cost;

void nathcat::cost::log_transaction(const httplib::Request &req,
                                    httplib::Response &res) {
  if (!req.has_param("group")) {
    res.status = httplib::StatusCode::BadRequest_400;
    res.set_content("Missing group param!", "text/plain");
    return;
  }

  int group = std::stoi(req.get_param_value("group"));
}
