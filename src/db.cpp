#include "jdbc/cppconn/driver.h"
#include "jdbc/mysql_driver.h"
#include <api/sql.hpp>
#include <costcat/costcat.hpp>
#include <fstream>
#include <memory>

namespace nathcat {
namespace cost {
sql::Driver *sqlDriver = sql::mysql::get_driver_instance();
struct config config{};
} // namespace cost
} // namespace nathcat

std::vector<struct nathcat::cost::balance>
nathcat::cost::util::get_balances(std::unique_ptr<sql::Connection> &db,
                                  int group) {
  std::unique_ptr<sql::PreparedStatement> pStmt{
      db->prepareStatement("select * from RunningTotals where `group` = ?")};
  pStmt->setInt(1, group);

  std::unique_ptr<sql::ResultSet> rs{pStmt->executeQuery()};

  return sqlwrapper::toArray<struct nathcat::cost::balance>(rs);
}

struct nathcat::cost::config nathcat::cost::get_config(std::string path) {
  std::ifstream f(path);
  nlohmann::json j = nlohmann::json::parse(f);

  return j.get<struct nathcat::cost::config>();
}

template <>
struct nathcat::cost::balance
nathcat::sqlwrapper::fromRow(std::unique_ptr<sql::ResultSet> &res) {
  return {res->getInt("user"), res->getInt("balance")};
}
