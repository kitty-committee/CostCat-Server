#include <api/sql.hpp>
#include <costcat/costcat.hpp>

template <>
struct nathcat::cost::debt
nathcat::sqlwrapper::fromRow(std::unique_ptr<sql::ResultSet> &res) {
  return {res->getInt("debtor"), res->getInt("creditor"),
          res->getInt("amount")};
}

template <>
struct nathcat::cost::total
nathcat::sqlwrapper::fromRow(std::unique_ptr<sql::ResultSet> &res) {
  return {res->getInt("user"), res->getInt("amount")};
}
