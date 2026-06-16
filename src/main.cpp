#include <api/api.hpp>
#include <httplib.h>
using namespace nathcat::api;

void hello_world(const httplib::Request &req, httplib::Response &res) {
  res.set_content("Hello world!", "text/plain");
}

int main() {
  Server server;

  server.registerEndpoint({"/", {hello_world, nullptr}});

  server.listen("0.0.0.0", 8080);
}
