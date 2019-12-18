// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_
#define AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_

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
    typedef plugin_handle type;
    typedef plugin<type>::event_result_t event_result_t;
    typedef plugin<type>::string_map_t string_map_t;
    typedef plugin<type>::arg_list_t arg_list_t;
    typedef plugin<type>::env_map_t env_map_t;

    plugin_handle();
    ~plugin_handle();

    void init(std::error_code &ec);
    void tear_down(std::error_code &ec);

    // client_connect is called by the plugin to handle the OpenVPN client connect
    // event
    event_result_t client_connect(arg_list_t const &args, env_map_t const &env,
                                  std::error_code &ec) const;

    // client_disconnect is called by the plugin to handle the OpenVPN client
    // disconnect event
    event_result_t client_disconnect(arg_list_t const &args, env_map_t const &env,
                                     std::error_code &ec) const;

    // learn_address is called by the plugin to handle the OpenVPN learn address
    // event
    event_result_t learn_address(arg_list_t const &args, env_map_t const &env,
                                 std::error_code &ec) const;

  private:
    // Utility method to create a proper event_result_t value
    static event_result_t make_event_result(plugin_results result, string_map_t &&string_map = {});
};

} // namespace autobahn::openvpn

#include "plugin_handle.ipp"
#endif // AUTOBAHN_SRC_PLUGIN_HANDLE_HPP_
