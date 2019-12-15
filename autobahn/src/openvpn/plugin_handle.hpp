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
  typedef plugin<plugin_handle>::event_result_t event_result_t;
  typedef plugin<plugin_handle>::string_map_t string_map_t;
  typedef plugin<plugin_handle>::arg_list_t arg_list_t;
  typedef plugin<plugin_handle>::env_map_t env_map_t;

  inline static event_result_t make_event_result(
      plugin_results result, string_map_t&& string_map = {}) {
    return plugin<plugin_handle>::make_event_result(result,
                                                    std::move(string_map));
  }

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
  event_result_t client_connect(arg_list_t const& args, env_map_t const& env,
                                std::error_code& ec) const {
    string_map_t values;
    values["config"] =
        "ifconfig-push 100.127.0.100 255.255.252.0\nifconfig-ipv6-push "
        "2a03:4000:6:11cd:bbbb::1100/112";

    return make_event_result(plugin_results::success, std::move(values));
  }

  // client_disconnect is called by the plugin to handle the OpenVPN client
  // disconnect event
  event_result_t client_disconnect(arg_list_t const& args, env_map_t const& env,
                                   std::error_code& ec) const {
    return make_event_result(plugin_results::success);
  }

  // learn_address is called by the plugin to handle the OpenVPN learn address
  // event
  event_result_t learn_address(arg_list_t const& args, env_map_t const& env,
                               std::error_code& ec) const {
    return make_event_result(plugin_results::success);
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_