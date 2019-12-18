// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_PLUGIN_HANDLE_IPP_
#define AUTOBAHN_SRC_PLUGIN_HANDLE_IPP_

#include <iostream>

#include "plugin_handle.hpp"

namespace autobahn::openvpn {

plugin_handle::plugin_handle()
{
    if (true) {
        // Do something stupid
    }
    else {
        // Do something even more stupid. :)
    }

    std::cout << "[autobahn-plugin] plugin_handle ctor called" << std::endl;
}

inline plugin_handle::~plugin_handle()
{
    std::cout << "[autobahn-plugin] plugin_handle dtor called" << std::endl;
}

inline void plugin_handle::init(std::error_code &ec)
{
    std::cout << "[autobahn-plugin] plugin_handle init called" << std::endl;
}

inline void plugin_handle::tear_down(std::error_code &ec)
{
    std::cout << "[autobahn-plugin] plugin_handle tear_down called" << std::endl;
    ec = autobahn::make_error_code(autobahn::error_codes::plugin_controller_gone);
}

// client_connect is called by the plugin to handle the OpenVPN client connect
// event
inline plugin_handle::event_result_t
plugin_handle::client_connect(plugin_handle::arg_list_t const &args,
                              plugin_handle::env_map_t const &env, std::error_code &ec) const
{
    string_map_t values;
    values["config"] = "ifconfig-push 100.127.0.100 255.255.252.0\nifconfig-ipv6-push "
                       "2a03:4000:6:11cd:bbbb::1100/112";

    return make_event_result(plugin_results::success, std::move(values));
}

// client_disconnect is called by the plugin to handle the OpenVPN client
// disconnect event
inline plugin_handle::event_result_t
plugin_handle::client_disconnect(plugin_handle::arg_list_t const &args,
                                 plugin_handle::env_map_t const &env, std::error_code &ec) const
{
    return make_event_result(plugin_results::success);
}

// learn_address is called by the plugin to handle the OpenVPN learn address
// event
inline plugin_handle::event_result_t
plugin_handle::learn_address(plugin_handle::arg_list_t const &args,
                             plugin_handle::env_map_t const &env, std::error_code &ec) const
{
    return make_event_result(plugin_results::success);
}

// Utility method to create a proper event_result_t value
inline plugin_handle::event_result_t plugin_handle::make_event_result(plugin_results result,
                                                                      string_map_t &&string_map)
{
    return std::make_tuple<>(result, std::move(string_map));
}

} // namespace autobahn::openvpn

#endif // AUTOBAHN_SRC_PLUGIN_HANDLE_IPP_