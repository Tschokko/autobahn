// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PLUGIN_SERVICE_STUB_HPP_
#define AUTOBAHN_SRC_OPENVPN_PLUGIN_SERVICE_STUB_HPP_

#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#include "zmq.hpp"

#include "../error_codes.hpp"
#include "../plugin_handler.hpp"
#include "../transport.hpp"
#include "plugin.hpp"
#include "plugin_types.hpp"

namespace autobahn::openvpn {

class plugin_service_stub {
 public:
  typedef std::tuple<bool, std::string> client_connect_result_t;

  client_connect_result_t client_connect(std::string const &common_name, std::error_code &ec);
  bool learn_address(std::string const &common_name, std::error_code &ec);
  void client_disconnect(std::string const &common_name, std::error_code &ec);

 private:
  client_connect_result_t make_client_connect_result(bool valid, std::string const &conf);
};

}  // namespace autobahn::openvpn

#include "plugin_service_stub.ipp"
#endif  // AUTOBAHN_SRC_OPENVPN_PLUGIN_SERVICE_STUB_HPP_
