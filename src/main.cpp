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

  nathcat::cost::config = nathcat::cost::get_config("Assets/costcat_conf.json");

  std::cout << "All configs loaded" << std::endl;

  Server server;

  server.registerEndpoints(
      {{"/transaction/new", {nullptr, nathcat::cost::log_transaction}},
       {"/balances", {nathcat::cost::get_balances, nullptr}},
       {"/debts", {nathcat::cost::get_debts, nullptr}}});

  std::cout << "Ready - Server is running on http://localhost:"
            << nathcat::cost::config.port << std::endl;
  server.listen("0.0.0.0", nathcat::cost::config.port);
}
