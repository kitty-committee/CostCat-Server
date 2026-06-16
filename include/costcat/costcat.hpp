/**
 * @file  costcat.hpp
 */

#ifndef _COSTCAT
#define _COSTCAT

#define AUTHCAT_CLIENT_MODE
#include <httplib.h>
#include <jdbc/cppconn/connection.h>
#include <memory>
namespace nathcat {
namespace cost {

static std::unique_ptr<sql::Connection> db;

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
 * @brief Endpoint handler which logs a new transaction.
 */
void log_transaction(const httplib::Request &req, httplib::Response &res);
} // namespace cost
} // namespace nathcat

#endif
