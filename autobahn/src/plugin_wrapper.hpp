// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_PLUGIN_WRAPPER_HPP_
#define AUTOBAHN_PLUGIN_WRAPPER_HPP_

#include <openvpn-plugin.h>
#include <memory>

namespace autobahn {

class plugin_wrapper {
 public:
  // PluginHandler(PluginHandler const&) = delete;
  // PluginHandler& operator=(PluginHandler const&) = delete;

  int open(struct openvpn_plugin_args_open_in const* args,
           struct openvpn_plugin_args_open_return* ret);
  void close();

  int handle(struct openvpn_plugin_args_func_in const* args,
             struct openvpn_plugin_args_func_return* retptr);

 private:
  // ProxyController controller_;

  // PluginHandler() {}

  int handle_tls_verify(struct openvpn_plugin_args_func_in const* args,
                        struct openvpn_plugin_args_func_return* retptr);
  int handle_auth_user_pass_verify(
      struct openvpn_plugin_args_func_in const* args,
      struct openvpn_plugin_args_func_return* retptr);
  int handle_client_connect(struct openvpn_plugin_args_func_in const* args,
                            struct openvpn_plugin_args_func_return* retptr);
  int handle_learn_address(struct openvpn_plugin_args_func_in const* args,
                           struct openvpn_plugin_args_func_return* retptr);
  int handle_client_disconnect(struct openvpn_plugin_args_func_in const* args,
                               struct openvpn_plugin_args_func_return* retptr);
};

}  // namespace autobahn

#include "plugin_wrapper.ipp"
#endif  // AUTOBAHN_PLUGIN_WRAPPER_HPP_