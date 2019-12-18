// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PLUGIN_HANDLE_HPP_
#define AUTOBAHN_SRC_OPENVPN_PLUGIN_HANDLE_HPP_

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
#include "plugin_service_stub.hpp"
#include "plugin_types.hpp"

namespace autobahn::openvpn {

class PluginHandle {
 public:
  typedef Plugin<PluginHandle>::EventResult EventResult;
  typedef Plugin<PluginHandle>::StringMap StringMap;
  typedef Plugin<PluginHandle>::ArgList ArgList;
  typedef Plugin<PluginHandle>::EnvMap EnvMap;
  typedef std::unique_ptr<plugin_service_stub> ServiceStubPtr;

  PluginHandle();
  ~PluginHandle();

  void Init(EnvMap &&env, std::error_code &ec);
  void TearDown(std::error_code &ec);

  // client_connect is called by the plugin to handle the OpenVPN client connect event
  EventResult HandleClientConnect(ArgList &&args, EnvMap &&env, std::error_code &ec) const;

  // client_disconnect is called by the plugin to handle the OpenVPN client
  // disconnect event
  EventResult HandleClientDisconnect(ArgList &&args, EnvMap &&env, std::error_code &ec) const;

  // learn_address is called by the plugin to handle the OpenVPN learn address
  // event
  EventResult HandleLearnAddress(ArgList &&args, EnvMap &&env, std::error_code &ec) const;

 private:
  std::shared_ptr<zmq::context_t> context_;
  std::shared_ptr<autobahn::zmq_transport> transport_;
  std::shared_ptr<autobahn::plugin_handler> handler_;
  std::thread listening_thread_;
  ServiceStubPtr service_;

  // Utility method to create a proper EventResult value
  static EventResult MakeEventResult(plugin_results result, StringMap &&string_map = {});
};

}  // namespace autobahn::openvpn

#include "plugin_handle.ipp"
#endif  // AUTOBAHN_SRC_OPENVPN_PLUGIN_HANDLE_HPP_
