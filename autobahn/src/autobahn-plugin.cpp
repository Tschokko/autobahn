// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#define ENABLE_CRYPTO 1

#include <openssl/ssl.h>
#include <openvpn/openvpn-plugin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <memory>
#include <string_view>

#include "openvpn/plugin.hpp"
#include "openvpn/plugin_handle.hpp"
#include "openvpn/plugin_types.hpp"

using autobahn::openvpn::plugin_events;

// typedef plugin<plugin_handle> plugin;
// class plugin : public autobahn::openvpn::plugin<autobahn::openvpn::plugin_handle> {
// };
using PluginImpl = autobahn::openvpn::Plugin<autobahn::openvpn::plugin_handle>;
using ArgList = PluginImpl::ArgList;
using EnvMap = PluginImpl::EnvMap;

// Convert the OpenVPN plugin argv items to a C++ vector.
inline static ArgList BuildArgs(const char *argv[]) {
  ArgList args;
  for (int i = 0; argv[i] != NULL; i++) {
    std::string arg{argv[i]};
    args.push_back(std::move(arg));
  }
  return args;
}

// Convert the OpenVPN plugin envp items to a C++ map.
inline static EnvMap BuildEnv(const char *envp[]) {
  EnvMap env;
  if (envp) {
    for (int i = 0; envp[i] != NULL; i++) {
      std::string s{envp[i]};
      auto pos = s.find_first_of('=');
      if (pos != std::string::npos) {
        auto key = s.substr(0, pos);
        auto val = s.substr(pos + 1);
        env[key] = val;
      }
    }
  }
  return env;
}

OPENVPN_EXPORT int openvpn_plugin_open_v3(const int v3structver,
                                          struct openvpn_plugin_args_open_in const *args,
                                          struct openvpn_plugin_args_open_return *ret) {
  // Check that we are API compatible.
  if (v3structver != OPENVPN_PLUGINv3_STRUCTVER) {
    std::cout << "[autobahn-plugin] incompatible plugin interface between this "
                 "plugin and OpenVPN"
              << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }
  // Run the plugin open method, to initialize the plugin handle and receive the
  // list of events we're interessted in.
  std::error_code ec;
  auto [events, handle] = PluginImpl::Open(BuildArgs(args->argv), BuildEnv(args->envp), ec);

  // If we receive an error code, log it to stdout and return a OpenVPN plugin
  // error. This stops our OpenVPN process, too.
  if (ec) {
    std::cout << "[autobahn-plugin] failed to open: " << ec.message() << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  // Tell our OpenVPN process which events we're interessted in.
  for (auto event : events) {
    ret->type_mask |= OPENVPN_PLUGIN_MASK(static_cast<int>(event));
  }

  // Store our plugin handle. We'll need it for further event processing.
  ret->handle = reinterpret_cast<openvpn_plugin_handle_t *>(handle.release());

  return OPENVPN_PLUGIN_FUNC_SUCCESS;
}

OPENVPN_EXPORT int openvpn_plugin_func_v3(const int version,
                                          struct openvpn_plugin_args_func_in const *args,
                                          struct openvpn_plugin_args_func_return *retptr) {
  // Call the plugin handle_event method to process the current event.
  std::error_code ec;
  auto [result, string_map] = PluginImpl::HandleEvent(
      static_cast<plugin_events>(args->type), BuildArgs(args->argv), BuildEnv(args->envp),
      reinterpret_cast<PluginImpl::HandleType *>(args->handle), ec);

  // If we receive an error code, log it and stop processing the event.
  if (ec) {
    std::cout << "[autobahn-plugin] failed to handle event: " << ec.message() << std::endl;
    return OPENVPN_PLUGIN_FUNC_ERROR;
  }

  // Populate the return_list with values, if a valid string_map is given.
  struct openvpn_plugin_string_list **el = retptr->return_list;
  for (auto const &[key, value] : string_map) {
    (*el) = reinterpret_cast<openvpn_plugin_string_list *>(
        malloc(sizeof(struct openvpn_plugin_string_list)));
    (*el)->name = strdup(key.c_str());
    (*el)->value = strdup(value.c_str());
    (*el)->next = NULL;
    el = &((*el)->next);
  }

  return static_cast<int>(result);
}

OPENVPN_EXPORT void openvpn_plugin_close_v1(openvpn_plugin_handle_t handle) {
  // Cast the given handle, execute the handle close (clean up) method and
  // delete the handle to free the allocated memory.
  auto h = reinterpret_cast<PluginImpl::HandleType *>(handle);
  std::error_code ec;
  PluginImpl::Close(h, ec);
  if (ec) {
    std::cout << "[autobahn-plugin] failure during close: " << ec.message() << std::endl;
  }
  delete h;
}
