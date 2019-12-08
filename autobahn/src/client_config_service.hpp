// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_CLIENT_CONFIG_SERVICE_HPP_
#define AUTOBAHN_SRC_CLIENT_CONFIG_SERVICE_HPP_

#include <map>
#include <memory>
#include <string>

#include "zmq.hpp"

#include "error_codes.hpp"
#include "openvpn/client_config.hpp"
#include "openvpn/client_config_builder.hpp"

namespace autobahn {

using autobahn::openvpn::client_config;
using autobahn::openvpn::client_config_builder;

class client_config_service {
 public:
  void add_or_update_client_config(std::string const& common_name,
                                   client_config const& config) {
    client_configs_[common_name] = config;
  }

  void add_or_update_client_config(std::string const& common_name,
                                   client_config&& config) {
    client_configs_.emplace(common_name, std::move(config));
  }

  void remove_client_config(std::string const& common_name) {
    client_configs_.erase(common_name);
  }

  std::tuple<bool, std::string> get_client_config(
      std::string const& common_name) {
    if (client_configs_.find(common_name) == client_configs_.end()) {
      return std::make_tuple<bool, std::string>(false, "");
    }

    // Build config string and move to the caller
    std::string config =
        client_config_builder::build(client_configs_[common_name]);
    // TODO(DGL) I don't understand why I have to move config explictly to the
    //           make_tuple operation. Otherwise I get a compiler error.
    return std::make_tuple<bool, std::string>(true, std::move(config));
  }

 private:
  std::map<std::string, client_config> client_configs_;
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_CLIENT_CONFIG_SERVICE_HPP_
