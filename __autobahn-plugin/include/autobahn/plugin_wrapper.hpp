// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_PLUGIN_WRAPPER_HPP_
#define AUTOBAHN_PLUGIN_WRAPPER_HPP_

#include <openvpn-plugin.h>
#include <memory>

#include "autobahn/proxy_controller.hpp"

namespace autobahn {

class PluginWrapper {
 public:
  // PluginHandler(PluginHandler const&) = delete;
  // PluginHandler& operator=(PluginHandler const&) = delete;

  int Open(struct openvpn_plugin_args_open_in const* args,
           struct openvpn_plugin_args_open_return* ret);
  void Close();

  int Handle(struct openvpn_plugin_args_func_in const* args,
             struct openvpn_plugin_args_func_return* retptr);
  int HandleTLSVerify(struct openvpn_plugin_args_func_in const* args,
                      struct openvpn_plugin_args_func_return* retptr);
  int HandleAuthUserPassVerify(struct openvpn_plugin_args_func_in const* args,
                               struct openvpn_plugin_args_func_return* retptr);
  int HandleClientConnect(struct openvpn_plugin_args_func_in const* args,
                          struct openvpn_plugin_args_func_return* retptr);
  int HandleLearnAddress(struct openvpn_plugin_args_func_in const* args,
                         struct openvpn_plugin_args_func_return* retptr);
  int HandleClientDisconnect(struct openvpn_plugin_args_func_in const* args,
                             struct openvpn_plugin_args_func_return* retptr);

 private:
  ProxyController controller_;

  // PluginHandler() {}
};

}  // namespace autobahn

#include "autobahn/plugin_wrapper.ipp"
#endif  // AUTOBAHN_PLUGIN_WRAPPER_HPP_