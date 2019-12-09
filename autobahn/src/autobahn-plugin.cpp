#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory>

#include <openssl/ssl.h>

#define ENABLE_CRYPTO 1
#include <openvpn/openvpn-plugin.h>

#include "openvpn/plugin_wrapper.hpp"

OPENVPN_EXPORT int openvpn_plugin_open_v3(
    const int v3structver, struct openvpn_plugin_args_open_in const *args,
    struct openvpn_plugin_args_open_return *ret) {
  // Check that we are API compatible
  if (v3structver != OPENVPN_PLUGINv3_STRUCTVER) {
    printf(
        "log_v3: ** ERROR ** Incompatible plug-in interface between this "
        "plug-in and OpenVPN\n");
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  auto plugin_wrapper = std::make_unique<autobahn::openvpn::plugin_wrapper>();
  auto rc = plugin_wrapper->open(args, ret);

  // Release our smart pointer und pass it to the openvpn plugin
  ret->handle =
      reinterpret_cast<openvpn_plugin_handle_t *>(plugin_wrapper.release());
  return rc;
}

OPENVPN_EXPORT int openvpn_plugin_func_v3(
    const int version, struct openvpn_plugin_args_func_in const *args,
    struct openvpn_plugin_args_func_return *retptr) {
  autobahn::openvpn::plugin_wrapper *plugin_wrapper =
      reinterpret_cast<autobahn::openvpn::plugin_wrapper *>(args->handle);
  return plugin_wrapper->handle(args, retptr);
}

OPENVPN_EXPORT void openvpn_plugin_close_v1(openvpn_plugin_handle_t handle) {
  autobahn::openvpn::plugin_wrapper *plugin_wrapper =
      reinterpret_cast<autobahn::openvpn::plugin_wrapper *>(handle);
  plugin_wrapper->close();
  delete plugin_wrapper;
}
