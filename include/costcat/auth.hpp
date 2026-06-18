#ifndef _COSTCAT_AUTH
#define _COSTCAT_AUTH

#include <AuthCat/auth.hpp>
#include <httplib.h>

namespace nathcat {
namespace cost {
nathcat::auth::User authenticate_connection(const httplib::Request &req);
}
} // namespace nathcat
#endif
