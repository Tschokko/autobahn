// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PLUGIN_HPP_
#define AUTOBAHN_SRC_OPENVPN_PLUGIN_HPP_

#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>

#include "plugin_types.hpp"

namespace autobahn::openvpn {

template <class H>
class plugin {
 public:
  typedef std::vector<plugin_events> event_list_t;
  typedef H handle_t;
  typedef std::unique_ptr<H> handle_ptr_t;
  typedef std::tuple<event_list_t, handle_ptr_t> open_result_t;
  typedef std::vector<std::string> arg_list_t;
  typedef std::map<std::string, std::string> env_map_t;
  typedef std::map<std::string, std::string> string_map_t;
  typedef std::tuple<plugin_results, string_map_t> event_result_t;

  // open is call when the plugin is started by the OpenVPN process
  // std::tuple<event_list_t, std::unique_ptr<H>>
  static open_result_t open(arg_list_t &&args, env_map_t &&env, std::error_code &ec) {
    // Define the OpenVPN events we want to listen for
    auto events = event_list_t{plugin_events::client_disconnect, plugin_events::learn_address,
                               plugin_events::client_connect_v2};

    // Create a new plugin handle pointer and initialize it. If an error code is
    // set, the plugin fails to start and the OpenVPN process will stop
    // immediately.
    auto handle = make_handle_ptr();
    handle->init(std::move(env), ec);

    return make_open_result(std::move(events), std::move(handle));
  }

  // close is called when the OpenVPN process is about to stop
  static void close(handle_t *const &handle, std::error_code &ec) { handle->tear_down(ec); }

  // handle_event is called when an OpenVPN event occurs. If we receive an
  // unregistered event (see method open) we raise an exception. If we receive
  // an unknown event we raise an exception, too. In both cases the plugin
  // fails and the OpenVPN process will terminate. Only properly registered
  // events are handled by this routine. If an error occurs during processing
  // the event, it will be only logged to stdout.
  static event_result_t handle_event(plugin_events event, arg_list_t &&args, env_map_t &&env,
                                     handle_t *const &handle, std::error_code &ec) {
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
        throw std::logic_error("received unregistered plugin event AUTH_USER_PASS_VERIFY");
      }
      case plugin_events::client_connect: {
        throw std::logic_error("received unregistered plugin event CLIENT_CONNECT");
      }
      case plugin_events::client_disconnect: {
        return handle->client_disconnect(std::move(args), std::move(env), ec);
      }
      case plugin_events::learn_address: {
        return handle->learn_address(std::move(args), std::move(env), ec);
      }
      case plugin_events::client_connect_v2: {
        return handle->client_connect(std::move(args), std::move(env), ec);
      }
      case plugin_events::tls_final: {
        throw std::logic_error("received unregistered plugin event TLS_FINAL");
      }
      case plugin_events::enable_pf: {
        throw std::logic_error("received unregistered plugin event ENABLE_PF");
      }
      case plugin_events::route_predown: {
        throw std::logic_error("received unregistered plugin event ROUTE_PREDOWN");
      }
      case plugin_events::n: {
        throw std::logic_error("received unregistered plugin event N");
      }
    }  // switch(event)

    // This exception should never be raised
    throw std::logic_error("received unknown plugin event");
  }

 private:
  static open_result_t make_open_result(event_list_t event_list, handle_ptr_t ptr) {
    return std::make_tuple<>(std::move(event_list), std::move(ptr));
  }
  static handle_ptr_t make_handle_ptr() { return std::make_unique<H>(); }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_PLUGIN_HPP_
