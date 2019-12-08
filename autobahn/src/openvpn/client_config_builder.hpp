// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_CLIENT_CONFIG_BUILDER_HPP_
#define AUTOBAHN_SRC_OPENVPN_CLIENT_CONFIG_BUILDER_HPP_

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"

#include "client_config.hpp"

namespace autobahn::openvpn {

using autobahn::openvpn::client_config;

class client_config_builder {
 public:
  static std::string build(client_config const& config) {
    std::stringstream ss;

    // ifconfig-push
    if (!config.ipv4_interface_config_.address().is_unspecified()) {
      ss << "ifconfig-push "
         << config.ipv4_interface_config_.address().to_string() << " "
         << config.ipv4_interface_config_.netmask().to_string() << "\n";
    }

    // ifconfig-push-ipv6
    if (!config.ipv6_interface_config_.address().is_unspecified()) {
      ss << "ifconfig-ipv6-push " << config.ipv6_interface_config_.to_string()
         << "\n";
    }

    // iroute list
    for (auto const& route : config.ipv4_networks_) {
      ss << "iroute " << route.address().to_string() << " "
         << route.netmask().to_string() << "\n";
    }

    // iroute-ipv6 list
    for (auto const& route : config.ipv6_networks_) {
      ss << "iroute-ipv6 " << route.to_string() << "\n";
    }

    // push route list
    for (auto const& route : config.ipv4_routes_) {
      ss << "push \"route " << route.address().to_string() << " "
         << route.netmask().to_string() << "\"\n";
    }

    // push route-ipv6 list
    for (auto const& route : config.ipv6_routes_) {
      ss << "push \"route-ipv6 " << route.to_string() << "\"\n";
    }

    return ss.str();
  }
};

}  // namespace autobahn::openvpn

// #include "client_config.ipp"
#endif  // AUTOBAHN_SRC_OPENVPN_CLIENT_CONFIG_BUILDER_HPP_
