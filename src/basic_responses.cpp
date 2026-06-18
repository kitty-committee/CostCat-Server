#include "costcat/costcat.hpp"
#include "httplib.h"
#include <costcat/costcat.hpp>
#include <nlohmann/json.hpp>

void nathcat::cost::success_response(httplib::Response &res) {
  nlohmann::json r = {{"status", "success"}};

  res.status = httplib::StatusCode::OK_200;
  res.set_content(r.dump(), "application/json");
}

void nathcat::cost::fail_response(httplib::Response &res, std::string message) {
  nlohmann::json r = {{"status", "fail"}, {"message", message}};

  res.status = httplib::StatusCode::OK_200;
  res.set_content(r.dump(), "application/json");
}
