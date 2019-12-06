// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_ERROR_CODES_HPP_
#define AUTOBAHN_SRC_ERROR_CODES_HPP_

#include <system_error>

namespace autobahn {
enum class ErrorCodes {
  // no 0
  RequestTimeout = 1,  // The request timed out
};
}  // namespace autobahn

namespace std {
template <>
struct is_error_code_enum<autobahn::ErrorCodes> : true_type {};
}  // namespace std

namespace autobahn {
std::error_code make_error_code(autobahn::ErrorCodes);
}

#endif  // AUTOBAHN_SRC_ERROR_CODES_HPP_
