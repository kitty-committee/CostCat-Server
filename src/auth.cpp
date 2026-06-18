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
