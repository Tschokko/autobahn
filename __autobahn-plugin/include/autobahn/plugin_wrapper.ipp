#include <iostream>

#include <openvpn-plugin.h>

namespace autobahn {

inline int PluginWrapper::Open(struct openvpn_plugin_args_open_in const* args,
                               struct openvpn_plugin_args_open_return* ret) {
  if (!controller_.Connect("abc")) {
    std::cerr << "Plugin failed to connect controller" << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  if (args->ssl_api != SSLAPI_OPENSSL) {
    // printf("This plug-in can only be used against OpenVPN with OpenSSL\n");
    std::cerr << "Plugin failed to load because it need OpenSSL" << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  /*OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY) |*/
  ret->type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_UP) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_DOWN) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_ROUTE_UP) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_IPCHANGE) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_TLS_VERIFY) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_CLIENT_CONNECT_V2) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_CLIENT_DISCONNECT) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_LEARN_ADDRESS) |
                   OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_TLS_FINAL);

  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline void PluginWrapper::Close() { std::cerr << "Close" << std::endl; }

inline int PluginWrapper::Handle(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  switch (args->type) {
    case OPENVPN_PLUGIN_TLS_VERIFY:
      return HandleTLSVerify(args, retptr);
    case OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY:
      return HandleAuthUserPassVerify(args, retptr);
    case OPENVPN_PLUGIN_CLIENT_CONNECT_V2:
      return HandleClientConnect(args, retptr);
    case OPENVPN_PLUGIN_LEARN_ADDRESS:
      return HandleLearnAddress(args, retptr);
    case OPENVPN_PLUGIN_CLIENT_DISCONNECT:
      return HandleClientDisconnect(args, retptr);
  }
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int PluginWrapper::HandleTLSVerify(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  // if (args->current_cert) {}
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int PluginWrapper::HandleAuthUserPassVerify(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int PluginWrapper::HandleClientConnect(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cerr << "HandleClientConnect" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int PluginWrapper::HandleLearnAddress(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cerr << "HandleLearnAddress" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int PluginWrapper::HandleClientDisconnect(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cerr << "HandleClientDisconnect" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

}  // namespace autobahn
