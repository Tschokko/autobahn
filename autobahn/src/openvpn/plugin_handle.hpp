// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_
#define AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#include "../error_codes.hpp"
#include "plugin_types.hpp"

namespace autobahn::openvpn {

class plugin_handle {
 public:
  plugin_handle() {
    std::cout << "[autobahn-plugin] plugin_handle ctor called" << std::endl;
  }
  ~plugin_handle() {
    std::cout << "[autobahn-plugin] plugin_handle dtor called" << std::endl;
  }

  void init(std::error_code& ec) {
    std::cout << "[autobahn-plugin] plugin_handle init called" << std::endl;
  }

  void tear_down(std::error_code& ec) {
    std::cout << "[autobahn-plugin] plugin_handle tear_down called"
              << std::endl;
    ec = autobahn::make_error_code(
        autobahn::error_codes::plugin_controller_gone);
  }

  // client_connect is called by the plugin to handle the OpenVPN client connect
  // event
  plugin_event_result_t client_connect(plugin_arg_list_t const& args,
                                       plugin_env_map_t const& env,
                                       std::error_code& ec) const {
    plugin_string_map_t values;
    values["config"] =
        "ifconfig-push 100.127.0.100 255.255.252.0\nifconfig-ipv6-push "
        "2a03:4000:6:11cd:bbbb::1100/112";

    return make_plugin_event_result(plugin_results::success, std::move(values));
  }

  // client_disconnect is called by the plugin to handle the OpenVPN client
  // disconnect event
  plugin_event_result_t client_disconnect(plugin_arg_list_t const& args,
                                          plugin_env_map_t const& env,
                                          std::error_code& ec) const {
    return make_plugin_event_result(plugin_results::success);
  }

  // learn_address is called by the plugin to handle the OpenVPN learn address
  // event
  plugin_event_result_t learn_address(plugin_arg_list_t const& args,
                                      plugin_env_map_t const& env,
                                      std::error_code& ec) const {
    return make_plugin_event_result(plugin_results::success);
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_