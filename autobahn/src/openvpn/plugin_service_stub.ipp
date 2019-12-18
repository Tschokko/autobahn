// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PLUGIN_SERVICE_STUB_IPP_
#define AUTOBAHN_SRC_OPENVPN_PLUGIN_SERVICE_STUB_IPP_

#include "plugin_service_stub.hpp"

namespace autobahn::openvpn {

using client_connect_result_t = plugin_service_stub::client_connect_result_t;

inline client_connect_result_t plugin_service_stub::client_connect(std::string const &common_name,
                                                                   std::error_code &ec) {
  return make_client_connect_result(true, "ifconfig-push 100.127.0.100 255.255.252.0");
}

inline bool plugin_service_stub::learn_address(std::string const &common_name,
                                               std::error_code &ec) {
  return true;
}

inline void plugin_service_stub::client_disconnect(std::string const &common_name,
                                                   std::error_code &ec) {}

inline client_connect_result_t plugin_service_stub::make_client_connect_result(
    bool valid, std::string const &conf) {
  return std::make_tuple<>(valid, conf);
}

}  // namespace autobahn::openvpn

// #include "plugin_handle.ipp"
#endif  // AUTOBAHN_SRC_OPENVPN_PLUGIN_SERVICE_STUB_IPP_
