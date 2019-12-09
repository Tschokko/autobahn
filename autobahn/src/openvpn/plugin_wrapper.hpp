// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_PLUGIN_WRAPPER_HPP_
#define AUTOBAHN_PLUGIN_WRAPPER_HPP_

#include <memory>

#include <openvpn/openvpn-plugin.h>

#include "zmq.hpp"

#include "../message.hpp"
#include "../plugin_handler.hpp"
#include "../transport.hpp"

namespace autobahn::openvpn {

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
  std::shared_ptr<zmq::context_t> context_;
  std::shared_ptr<autobahn::zmq_transport> transport_;
  std::shared_ptr<autobahn::plugin_handler> handler_;
  std::thread listening_thread_;

  // PluginHandler() {}
  const char* get_env(const char* name, const char* envp[]);

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

}  // namespace autobahn::openvpn

#include "plugin_wrapper.ipp"
#endif  // AUTOBAHN_PLUGIN_WRAPPER_HPP_