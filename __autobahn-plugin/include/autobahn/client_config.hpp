// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_CLIENT_CONFIG_HPP_
#define AUTOBAHN_CLIENT_CONFIG_HPP_

#include <sstream>
#include <string>
#include <vector>

class ClientConfig {
 public:
  void AddIPv4Route(const std::string& ipv4_route) {
    push_ipv4_routes_.push_back(ipv4_route);
  }

  std::string DumpOpenVPNConfig() const {
    std::stringstream dump;

    // Push IPv4 routes
    for (auto const& route : push_ipv4_routes_) {
      dump << "push \"route " << route << "\"\n";
    }

    return dump.str();
  }

 private:
  std::vector<std::string> push_ipv4_routes_;
};

#endif  // AUTOBAHN_CLIENT_CONFIG_HPP_