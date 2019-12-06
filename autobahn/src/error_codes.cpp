// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <string>

#include "error_codes.hpp"

namespace {  // anonymous namespace

struct AutobahnErrorCodesCategory : std::error_category {
  const char* name() const noexcept override;
  std::string message(int ev) const override;
};

const char* AutobahnErrorCodesCategory::name() const noexcept {
  return "autobahn";
}

std::string AutobahnErrorCodesCategory::message(int ev) const {
  switch (static_cast<autobahn::ErrorCodes>(ev)) {
    case autobahn::ErrorCodes::RequestTimeout:
      return "request timed out";

    default:
      return "(unrecognized error)";
  }
}

const AutobahnErrorCodesCategory autobahn_error_codes_category{};

}  // anonymous namespace

namespace autobahn {
std::error_code make_error_code(autobahn::ErrorCodes e) {
  return {static_cast<int>(e), autobahn_error_codes_category};
}
}  // namespace autobahn
