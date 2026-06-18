/**
 * @file  costcat.hpp
 */

#ifndef _COSTCAT
#define _COSTCAT

#include <nlohmann/json.hpp>
#include <vector>
#define AUTHCAT_CLIENT_MODE
#include <httplib.h>
#include <jdbc/cppconn/connection.h>

namespace nathcat {
namespace cost {

extern sql::Driver *sqlDriver;

struct config {
  std::string dbUrl;
  std::string dbUsername;
  std::string dbPassword;
};

extern struct config config;

struct debt {
  int debtor;
  int creditor;
  int value;
};

struct total {
  int user;
  int amount;
};

/**
 * @class transaction_request
 * @brief Specifies the request format for the body of requests to
 * log_transaction
 *
 */
struct transaction_request {
  int amount;
  std::vector<int> payees;
  std::string description;
};

void to_json(nlohmann::json &j, const struct transaction_request &t);
void from_json(const nlohmann::json &j, struct transaction_request &t);
void from_json(const nlohmann::json &j, struct config &c);

struct config get_config(std::string path);

void success_response(httplib::Response &res);
void fail_response(httplib::Response &res, std::string message);

/**
 * @brief Endpoint handler which logs a new transaction.
 */
void log_transaction(const httplib::Request &req, httplib::Response &res);
} // namespace cost
} // namespace nathcat

#endif
