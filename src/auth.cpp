#define AUTHCAT_CLIENT_MODE
#include <AuthCat/auth.hpp>
#include <AuthCat/db/Credentials.hpp>
#include <api/api.hpp>
#include <costcat/auth.hpp>
#include <httplib.h>

nathcat::auth::User
nathcat::cost::authenticate_connection(const httplib::Request &req) {
  std::string authCookie = nathcat::api::get_cookie(req, AUTHCAT_COOKIE_NAME);

  if (authCookie == "") {
    throw auth::AuthFailed();
  }

  auth::Credentials_Token token(authCookie);
  nathcat::auth::User user;

  user = nathcat::auth::authenticate(token);
  return user;
}

nathcat::auth::User
nathcat::cost::authenticate_request(const httplib::Request &req,
                                    httplib::Response &res) {

  nathcat::auth::User user;
  try {
    user = authenticate_connection(req);
  } catch (auth::AuthFailed &e) {
    // Authentication has failed, return a 403 error
    //
    std::cout << "\tFailed authentication." << std::endl;

    res.status = httplib::StatusCode::Forbidden_403;
    res.set_content("Not logged in", "text/plain");
    throw e;
  } catch (std::exception &e) {
    // An unspecified exception has occurred, return a 500 error
    //
    std::cout << "\tException during authentication." << std::endl;
    std::cerr << e.what() << std::endl;

    res.status = httplib::StatusCode::InternalServerError_500;
    res.set_content(e.what(), "text/plain");
    throw e;
  }

  return user;
}
