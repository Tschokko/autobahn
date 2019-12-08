// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_CLIENT_CONFIG_HPP_
#define AUTOBAHN_SRC_OPENVPN_CLIENT_CONFIG_HPP_

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"
#include "boost/move/move.hpp"

namespace autobahn::openvpn {

using boost::asio::ip::network_v4;
using boost::asio::ip::network_v6;

class client_config {
 public:
  client_config() {}

  // TODO(DGL) Move operations leads to compiler errors with boost. :-((
  client_config(client_config const& other) {
    std::cerr << "client_config ctor w/ copy" << std::endl;
    ipv4_interface_config_ = other.ipv4_interface_config_;
    ipv6_interface_config_ = other.ipv6_interface_config_;
    ipv4_routes_ = other.ipv4_routes_;
    ipv6_routes_ = other.ipv6_routes_;
    ipv4_networks_ = other.ipv4_networks_;
    ipv6_networks_ = other.ipv6_networks_;
  }

  client_config(client_config&& other) {
    std::cerr << "client_config ctor w/ move" << std::endl;
    ipv4_interface_config_ = std::move(other.ipv4_interface_config_);
    ipv6_interface_config_ = std::move(other.ipv6_interface_config_);
    ipv4_routes_ = std::move(other.ipv4_routes_);
    ipv6_routes_ = std::move(other.ipv6_routes_);
    ipv4_networks_ = std::move(other.ipv4_networks_);
    ipv6_networks_ = std::move(other.ipv6_networks_);
  }

  client_config& operator=(client_config const& other) {
    if (this == &other) {
      return *this;
    }

    std::cerr << "client_config copy operator w/ copy" << std::endl;
    ipv4_interface_config_ = other.ipv4_interface_config_;
    ipv6_interface_config_ = other.ipv6_interface_config_;
    ipv4_routes_ = other.ipv4_routes_;
    ipv6_routes_ = other.ipv6_routes_;
    ipv4_networks_ = other.ipv4_networks_;
    ipv6_networks_ = other.ipv6_networks_;

    return *this;
  }

  client_config& operator=(client_config&& other) {
    if (this == &other) {
      return *this;
    }

    std::cerr << "client_config copy operator w/ move" << std::endl;
    ipv4_interface_config_ = std::move(other.ipv4_interface_config_);
    ipv6_interface_config_ = std::move(other.ipv6_interface_config_);
    ipv4_routes_ = std::move(other.ipv4_routes_);
    ipv6_routes_ = std::move(other.ipv6_routes_);
    ipv4_networks_ = std::move(other.ipv4_networks_);
    ipv6_networks_ = std::move(other.ipv6_networks_);

    return *this;
  }

  network_v4 ipv4_interface_config() const { return ipv4_interface_config_; }
  void set_ipv4_interface_config(network_v4 const& addr) {
    ipv4_interface_config_ = addr;
  }

  network_v6 ipv6_interface_config() const { return ipv6_interface_config_; }
  void set_ipv6_interface_config(network_v6 const& addr) {
    ipv6_interface_config_ = addr;
  }

  void add_ipv4_route(network_v4 const& route) {
    ipv4_routes_.push_back(route);
  }

  void add_ipv6_route(network_v6 const& route) {
    ipv6_routes_.push_back(route);
  }

  void add_ipv4_network(network_v4 const& network) {
    ipv4_networks_.push_back(network);
  }

  void add_ipv6_network(network_v6 const& network) {
    ipv6_networks_.push_back(network);
  }

 private:
  friend class client_config_builder;

  network_v4 ipv4_interface_config_;
  network_v6 ipv6_interface_config_;
  std::vector<network_v4> ipv4_routes_;
  std::vector<network_v6> ipv6_routes_;
  std::vector<network_v4> ipv4_networks_;
  std::vector<network_v6> ipv6_networks_;
};

}  // namespace autobahn::openvpn

// #include "client_config.ipp"
#endif  // AUTOBAHN_SRC_OPENVPN_CLIENT_CONFIG_HPP_
