#include <iostream>

#include <openvpn-plugin.h>

namespace autobahn {

inline int plugin_wrapper::open(struct openvpn_plugin_args_open_in const* args,
                                struct openvpn_plugin_args_open_return* ret) {
  /*if (!controller_.Connect("abc")) {
    std::cerr << "Plugin failed to connect controller" << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }*/

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

inline void plugin_wrapper::close() { std::cerr << "Close" << std::endl; }

inline int plugin_wrapper::handle(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  switch (args->type) {
    case OPENVPN_PLUGIN_TLS_VERIFY:
      return handle_tls_verify(args, retptr);
    case OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY:
      return handle_auth_user_pass_verify(args, retptr);
    case OPENVPN_PLUGIN_CLIENT_CONNECT_V2:
      return handle_client_connect(args, retptr);
    case OPENVPN_PLUGIN_LEARN_ADDRESS:
      return handle_learn_address(args, retptr);
    case OPENVPN_PLUGIN_CLIENT_DISCONNECT:
      return handle_client_disconnect(args, retptr);
  }
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_tls_verify(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  // if (args->current_cert) {}
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_auth_user_pass_verify(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_client_connect(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cerr << "handle_client_connect" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_learn_address(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cerr << "handle_learn_address" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_client_disconnect(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cerr << "handle_client_disconnect" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

}  // namespace autobahn
