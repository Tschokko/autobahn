#include <iostream>

namespace autobahn::openvpn {

inline const char* plugin_wrapper::get_env(const char* name,
                                           const char* envp[]) {
  if (envp) {
    int i;
    const int namelen = strlen(name);
    for (i = 0; envp[i]; ++i) {
      if (!strncmp(envp[i], name, namelen)) {
        const char* cp = envp[i] + namelen;
        if (*cp == '=') {
          return cp + 1;
        }
      }
    }
  }
  return NULL;
}

inline int plugin_wrapper::open(struct openvpn_plugin_args_open_in const* args,
                                struct openvpn_plugin_args_open_return* ret) {
  /*if (!controller_.Connect("abc")) {
    std::cout << "Plugin failed to connect controller" << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }*/

  if (args->ssl_api != SSLAPI_OPENSSL) {
    // printf("This plug-in can only be used against OpenVPN with OpenSSL\n");
    std::cout << "Plugin failed to load because it need OpenSSL" << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  // init our wrapper
  std::cout << "prepare our connection to the server" << std::endl;
  context_ = std::make_shared<zmq::context_t>(1);
  transport_ = std::make_shared<autobahn::zmq_transport>(context_);
  handler_ = std::make_shared<autobahn::plugin_handler>();

  std::cout << "connect to our server" << std::endl;
  transport_->attach(handler_);
  transport_->connect("ipc:///tmp/autobahn");

  std::cout << "start our listening thread" << std::endl;
  listening_thread_ = std::thread([&] { transport_->listen(); });

  std::cout << "finalize open function" << std::endl;

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

inline void plugin_wrapper::close() {
  std::cout << "Close" << std::endl;
  transport_->shutdown(10);
}

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
  std::cout << "handle_client_connect" << std::endl;

  // Request client connect
  std::string common_name = get_env("common_name", args->envp);
  std::error_code ec;
  auto reply = handler_->request_client_connect(common_name, ec);

  if (!reply.valid()) {
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  // OpenVPN plugin client config response
  (*retptr->return_list) = reinterpret_cast<openvpn_plugin_string_list*>(
      malloc(sizeof(struct openvpn_plugin_string_list)));
  (*retptr->return_list)->next = NULL;
  (*retptr->return_list)->name = strdup("config");
  (*retptr->return_list)->value = strdup(reply.config().c_str());

  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_learn_address(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cout << "handle_learn_address" << std::endl;

  std::string operation = args->argv[1];
  std::string address = args->argv[2];
  std::string common_name = args->argv[3];

  std::error_code ec;
  auto reply = handler_->request_learn_address(
      autobahn::learn_address_operations_from_string(operation), address,
      common_name, ec);

  if (!reply.learned()) {
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  /*size_t i;
  printf("ARGV\n");
  for (i = 0; args->argv[i] != NULL; ++i) {
    printf("%d '%s'\n", (int)i, args->argv[i]);
  }

  printf("ENVP\n");
  for (i = 0; args->envp[i] != NULL; ++i) {
    printf("%d '%s'\n", (int)i, args->envp[i]);
  }*/

  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

inline int plugin_wrapper::handle_client_disconnect(
    struct openvpn_plugin_args_func_in const* args,
    struct openvpn_plugin_args_func_return* retptr) {
  std::cout << "handle_client_disconnect" << std::endl;
  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

}  // namespace autobahn::openvpn
