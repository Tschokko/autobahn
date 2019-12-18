// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PLUGIN_HANDLE_IPP_
#define AUTOBAHN_SRC_OPENVPN_PLUGIN_HANDLE_IPP_

#include <iostream>

#include "plugin_handle.hpp"

namespace autobahn::openvpn {

using EventResult = PluginHandle::EventResult;
using ArgList = PluginHandle::ArgList;
using EnvMap = PluginHandle::EnvMap;

inline PluginHandle::PluginHandle() {
  std::cout << "[autobahn-plugin] plugin_handle ctor called" << std::endl;
}

inline PluginHandle::~PluginHandle() {
  std::cout << "[autobahn-plugin] plugin_handle dtor called" << std::endl;
}

inline void PluginHandle::Init(EnvMap &&env, std::error_code &ec) {
  std::cout << "[autobahn-plugin] plugin_handle init called" << std::endl;

  context_ = std::make_shared<zmq::context_t>(1);
  transport_ = std::make_shared<autobahn::zmq_transport>(context_);
  handler_ = std::make_shared<autobahn::plugin_handler>();

  std::cout << "[autobahn-plugin] connect to our server" << std::endl;
  transport_->attach(handler_);
  transport_->connect(env["AUTOBAHN_SERVER_ADDRESS"]);

  // Initialize our service stub
  service_ = std::make_unique<plugin_service_stub>();

  std::cout << "[autobahn-plugin] start our listening thread" << std::endl;
  listening_thread_ = std::thread([&] { transport_->listen(); });
}

inline void PluginHandle::TearDown(std::error_code &ec) {
  std::cout << "[autobahn-plugin] plugin_handle tear_down called" << std::endl;
  // ec = autobahn::make_error_code(autobahn::error_codes::plugin_controller_gone);
  transport_->shutdown(10);
  listening_thread_.join();
}

// client_connect is called by the plugin to handle the OpenVPN client connect
// event
inline EventResult PluginHandle::HandleClientConnect(ArgList &&args, EnvMap &&env,
                                                     std::error_code &ec) const {
  /*values["config"] = "ifconfig-push 100.127.0.100 255.255.252.0\nifconfig-ipv6-push "
                     "2a03:4000:6:11cd:bbbb::1100/112";
  */

  auto result = service_->client_connect(env["common_name"], ec);

  // If we have an error or the service rejected the client connect request, we return a failure.
  if (ec || !std::get<0>(result)) {
    return MakeEventResult(plugin_results::failure);
  }

  // Pouplate our string map with the given config.
  StringMap values;
  values["config"] = std::get<1>(result);

  return MakeEventResult(plugin_results::success, std::move(values));
}

// client_disconnect is called by the plugin to handle the OpenVPN client
// disconnect event
inline EventResult PluginHandle::HandleClientDisconnect(ArgList &&args, EnvMap &&env,
                                                        std::error_code &ec) const {
  return MakeEventResult(plugin_results::success);
}

// learn_address is called by the plugin to handle the OpenVPN learn address
// event
inline EventResult PluginHandle::HandleLearnAddress(ArgList &&args, EnvMap &&env,
                                                    std::error_code &ec) const {
  return MakeEventResult(plugin_results::success);
}

// Utility method to create a proper EventResult value
inline EventResult PluginHandle::MakeEventResult(plugin_results result, StringMap &&string_map) {
  return std::make_tuple<>(result, std::move(string_map));
}

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_PLUGIN_HANDLE_IPP_
