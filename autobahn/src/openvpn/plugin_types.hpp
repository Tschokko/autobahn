// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PLUGIN_TYPES_HPP_
#define AUTOBAHN_SRC_OPENVPN_PLUGIN_TYPES_HPP_

#include "openvpn/openvpn-plugin.h"

namespace autobahn::openvpn {

enum class plugin_events : int {
  up = OPENVPN_PLUGIN_UP,
  down = OPENVPN_PLUGIN_DOWN,
  route_up = OPENVPN_PLUGIN_ROUTE_UP,
  ipchange = OPENVPN_PLUGIN_IPCHANGE,
  tls_verify = OPENVPN_PLUGIN_TLS_VERIFY,
  auth_user_pass_verify = OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY,
  client_connect = OPENVPN_PLUGIN_CLIENT_CONNECT,
  client_disconnect = OPENVPN_PLUGIN_CLIENT_DISCONNECT,
  learn_address = OPENVPN_PLUGIN_LEARN_ADDRESS,
  client_connect_v2 = OPENVPN_PLUGIN_CLIENT_CONNECT_V2,
  tls_final = OPENVPN_PLUGIN_TLS_FINAL,
  enable_pf = OPENVPN_PLUGIN_ENABLE_PF,
  route_predown = OPENVPN_PLUGIN_ROUTE_PREDOWN,
  n = OPENVPN_PLUGIN_N,
};

enum class plugin_results : int {
  success = OPENVPN_PLUGIN_FUNC_SUCCESS,
  failure = OPENVPN_PLUGIN_FUNC_ERROR,
  deferred = OPENVPN_PLUGIN_FUNC_DEFERRED,
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_PLUGIN_TYPES_HPP_
