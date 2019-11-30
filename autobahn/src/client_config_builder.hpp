// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef CLIENT_CONFIG_BUILDER_HPP_
#define CLIENT_CONFIG_BUILDER_HPP_

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"

namespace autobahn::openvpn {

using boost::asio::ip::network_v4;
using boost::asio::ip::network_v6;

class ClientConfigBuilder {
 public:
  void SetIPv4InterfaceConfig(network_v4 const& addr) {
    ipv4_interface_config_ = addr;
  }

  void SetIPv6InterfaceConfig(network_v6 const& addr) {
    ipv6_interface_config_ = addr;
  }

  void AddIPv4Route(network_v4 const& route) { ipv4_routes_.push_back(route); }

  void AddIPv6Route(network_v6 const& route) { ipv6_routes_.push_back(route); }

  void AddIPv4Network(network_v4 const& network) {
    ipv4_networks_.push_back(network);
  }

  void AddIPv6Network(network_v6 const& network) {
    ipv6_networks_.push_back(network);
  }

  std::string BuildConfig() const {
    std::stringstream sout;

    // ifconfig-push
    if (!ipv4_interface_config_.address().is_unspecified()) {
      sout << "ifconfig-push " << ipv4_interface_config_.address().to_string()
           << " " << ipv4_interface_config_.netmask().to_string() << "\n";
    }

    // ifconfig-push-ipv6
    if (!ipv6_interface_config_.address().is_unspecified()) {
      sout << "ifconfig-ipv6-push " << ipv6_interface_config_.to_string()
           << "\n";
    }

    // iroute list
    for (auto const& route : ipv4_networks_) {
      sout << "iroute " << route.address().to_string() << " "
           << route.netmask().to_string() << "\n";
    }

    // iroute-ipv6 list
    for (auto const& route : ipv6_networks_) {
      sout << "iroute-ipv6 " << route.to_string() << "\n";
    }

    // push route list
    for (auto const& route : ipv4_routes_) {
      sout << "push \"route " << route.address().to_string() << " "
           << route.netmask().to_string() << "\"\n";
    }

    // push route-ipv6 list
    for (auto const& route : ipv6_routes_) {
      sout << "push \"route-ipv6 " << route.to_string() << "\"\n";
    }

    return sout.str();
  }

 private:
  network_v4 ipv4_interface_config_;
  network_v6 ipv6_interface_config_;
  std::vector<network_v4> ipv4_routes_;
  std::vector<network_v6> ipv6_routes_;
  std::vector<network_v4> ipv4_networks_;
  std::vector<network_v6> ipv6_networks_;
};

}  // namespace autobahn::openvpn

// #include "client_config.ipp"
#endif  // CLIENT_CONFIG_BUILDER_HPP_