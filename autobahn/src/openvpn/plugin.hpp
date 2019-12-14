// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_PLUGIN_HPP_
#define AUTOBAHN_SRC_PLUGIN_HPP_

#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#include "plugin_types.hpp"

namespace autobahn::openvpn {

template <class H>
class plugin {
 public:
  typedef H handle_t;

  // open is call when the plugin is started by the OpenVPN process
  static std::tuple<plugin_event_list_t, std::unique_ptr<H>> open(
      plugin_arg_list_t const& args, plugin_env_map_t const& env,
      std::error_code& ec) {
    // Define the OpenVPN events we want to listen for
    auto events = plugin_event_list_t{plugin_events::client_disconnect,
                                      plugin_events::learn_address,
                                      plugin_events::client_connect_v2};

    // Create a new plugin handle and initialize it. If an error code is set,
    // the plugin fails to start and the OpenVPN process will stop immediately.
    auto handle = std::make_unique<H>();
    handle->init(ec);

    return std::make_tuple<>(std::move(events), std::move(handle));
  }

  // close is called when the OpenVPN process is about to stop
  static void close(handle_t* const& handle, std::error_code& ec) {
    handle->tear_down(ec);
  }

  // handle_event is called when an OpenVPN event occurs. If we receive an
  // unregistered event (see method open) we raise an exception. If we receive
  // an unknown event we raise an exception, too. In both cases the plugin
  // fails and the OpenVPN process will terminate. Only properly registered
  // events are handled by this routine. If an error occurs during processing
  // the event, it will be only logged to stdout.
  static plugin_event_result_t handle_event(plugin_events event,
                                            plugin_arg_list_t const& args,
                                            plugin_env_map_t const& env,
                                            handle_t* const& handle,
                                            std::error_code& ec) {
    switch (event) {
      case plugin_events::up: {
        throw std::logic_error("received unregistered plugin event UP");
      }
      case plugin_events::down: {
        throw std::logic_error("received unregistered plugin event DOWN");
      }
      case plugin_events::route_up: {
        throw std::logic_error("received unregistered plugin event ROUTE_UP");
      }
      case plugin_events::ipchange: {
        throw std::logic_error("received unregistered plugin event IPCHANGE");
      }
      case plugin_events::tls_verify: {
        throw std::logic_error("received unregistered plugin event TLS_VERIFY");
      }
      case plugin_events::auth_user_pass_verify: {
        throw std::logic_error(
            "received unregistered plugin event AUTH_USER_PASS_VERIFY");
      }
      case plugin_events::client_connect: {
        throw std::logic_error(
            "received unregistered plugin event CLIENT_CONNECT");
      }
      case plugin_events::client_disconnect: {
        return handle->client_disconnect(args, env, ec);
      }
      case plugin_events::learn_address: {
        return handle->learn_address(args, env, ec);
      }
      case plugin_events::client_connect_v2: {
        return handle->client_connect(args, env, ec);
      }
      case plugin_events::tls_final: {
        throw std::logic_error("received unregistered plugin event TLS_FINAL");
      }
      case plugin_events::enable_pf: {
        throw std::logic_error("received unregistered plugin event ENABLE_PF");
      }
      case plugin_events::route_predown: {
        throw std::logic_error(
            "received unregistered plugin event ROUTE_PREDOWN");
      }
      case plugin_events::n: {
        throw std::logic_error("received unregistered plugin event N");
      }
    }

    // This exception should never be raised
    throw std::logic_error("received unknown plugin event");
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_PLUGIN_HPP_