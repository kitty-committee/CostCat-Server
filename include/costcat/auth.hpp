#ifndef _COSTCAT_AUTH
#define _COSTCAT_AUTH

#include <AuthCat/auth.hpp>
#include <httplib.h>

namespace nathcat {
namespace cost {
nathcat::auth::User authenticate_connection(const httplib::Request &req);

nathcat::auth::User authenticate_request(const httplib::Request &req,
                                         httplib::Response &res);
} // namespace cost
} // namespace nathcat
#endif
