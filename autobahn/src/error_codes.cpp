// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <string>

#include "error_codes.hpp"

namespace {  // anonymous namespace

struct autobahn_error_category : std::error_category {
  const char* name() const noexcept override;
  std::string message(int ev) const override;
};

const char* autobahn_error_category::name() const noexcept {
  return "autobahn";
}

std::string autobahn_error_category::message(int ev) const {
  switch (static_cast<autobahn::error_codes>(ev)) {
    case autobahn::error_codes::request_timeout:
      return "request timed out";
    case autobahn::error_codes::no_client_config:
      return "no client config";
    case autobahn::error_codes::plugin_controller_unreachable:
      return "plugin controller unreachable";
    case autobahn::error_codes::plugin_controller_gone:
      return "plugin controller gone";
    default:
      return "(unrecognized error)";
  }
}

const autobahn_error_category g_autobahn_error_category{};

}  // anonymous namespace

namespace autobahn {
std::error_code make_error_code(autobahn::error_codes ec) {
  return {static_cast<int>(ec), g_autobahn_error_category};
}
}  // namespace autobahn
