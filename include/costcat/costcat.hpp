/**
 * @file  costcat.hpp
 */

#ifndef _COSTCAT
#define _COSTCAT

#include <cstdint>
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

struct balance {
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

/**
 * @class debt
 * @brief A representation of a debt. i.e. the debtor is to pay the creditor an
 * amount.
 *
 */
struct debt {
  int creditor;
  int debtor;
  int amount;
};

void to_json(nlohmann::json &j, const struct debt &d);
void to_json(nlohmann::json &j, const struct balance &b);
void to_json(nlohmann::json &j, const struct transaction_request &t);
void from_json(const nlohmann::json &j, struct transaction_request &t);
void from_json(const nlohmann::json &j, struct config &c);

struct config get_config(std::string path);

void success_response(httplib::Response &res);
void fail_response(httplib::Response &res, std::string message);

namespace util {
/**
 * @brief Get a group's balances from the database
 */
std::vector<struct balance> get_balances(std::unique_ptr<sql::Connection> &db,
                                         int group);
} // namespace util

/**
 * @brief Find a set of debts which restore equilibrium in a set of balances
 * through a single collector.
 *
 * @param collector The collector of the debts
 * @param balances The set of balances to simplify
 */
std::vector<struct debt>
simplify_with_collector(int collector, std::vector<struct balance> balances);

/**
 * @brief Determines subsets of elements from set which sum to zero
 *
 * @return A 2D array, where each sub-array contains the indexes of elements
 * from set which, when summed, amount to zero.
 */
std::vector<std::vector<struct balance>>
zero_subset_sum(std::vector<struct balance> set);

/**
 * @brief Extract a subset from a set using a bitmask
 *
 * @param V The bitmask. 1 indicates an index to include
 * @param set The set to take values from
 */
std::vector<struct balance> subset(std::uint64_t V,
                                   std::vector<struct balance> set);

/**
 * @brief Endpoint handler which logs a new transaction.
 */
void log_transaction(const httplib::Request &req, httplib::Response &res);

/**
 * @brief Endpoint handler which gets the current balances of users in a group
 */
void get_balances(const httplib::Request &req, httplib::Response &res);

/**
 * @brief Endpoint which gets the simplified debts which can repay the balances
 * of a group
 */
void get_debts(const httplib::Request &req, httplib::Response &res);
} // namespace cost
} // namespace nathcat

#endif
