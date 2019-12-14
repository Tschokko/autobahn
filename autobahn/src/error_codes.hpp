// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_ERROR_CODES_HPP_
#define AUTOBAHN_SRC_ERROR_CODES_HPP_

#include <system_error>

namespace autobahn {
enum class error_codes {
  // no 0
  request_timeout = 1,  // The request timed out

  // server controller errors
  no_client_config = 1000,

  // plugin errors
  plugin_controller_unreachable = 2000,
  plugin_controller_gone = 2001,
};
}  // namespace autobahn

namespace std {
template <>
struct is_error_code_enum<autobahn::error_codes> : true_type {};
}  // namespace std

namespace autobahn {
std::error_code make_error_code(autobahn::error_codes);
}

#endif  // AUTOBAHN_SRC_ERROR_CODES_HPP_
