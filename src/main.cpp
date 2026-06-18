#include "jdbc/cppconn/connection.h"
#include "jdbc/mysql_driver.h"
#include <api/api.hpp>
#include <costcat/costcat.hpp>
#include <httplib.h>
#include <memory>
#define AUTHCAT_CLIENT_MODE
#include <AuthCat/auth.hpp>
using namespace nathcat::api;

int main() {
  nathcat::auth::clientConfig =
      nathcat::auth::getConfig<struct nathcat::auth::ClientConfig>(
          "Assets/authcat_conf.json");

  std::cout << "Got AuthCat config, hostUrl is "
            << nathcat::auth::clientConfig.hostUrl << std::endl;

  sql::Driver *driver = sql::mysql::get_driver_instance();

  nathcat::cost::db = std::unique_ptr<sql::Connection>{
      driver->connect("localhost:3306", "root", "")};
  nathcat::cost::db->setSchema("CostCat");

  std::cout << "Connected to CostCat DB" << std::endl;

  Server server;

  server.registerEndpoint(
      {"/transaction/new", {nullptr, nathcat::cost::log_transaction}});

  server.listen("0.0.0.0", 8080);
  std::cout << "Ready" << std::endl;
}
