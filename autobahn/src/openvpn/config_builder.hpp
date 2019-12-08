// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_CONFIG_BUILDER_HPP_
#define AUTOBAHN_SRC_OPENVPN_CONFIG_BUILDER_HPP_

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"

#include "config.hpp"

namespace autobahn::openvpn {

using boost::asio::ip::network_v4;
using boost::asio::ip::network_v6;

class config_builder {
 public:
  static std::vector<std::string> build_args(config const& config) {
    std::vector<std::string> args;
    for (auto const& arg : config.flags_) {
      args.push_back("--" + arg);
    }
    for (const auto& [arg, value] : config.values_) {
      args.push_back("--" + arg + " " + value);
    }
    return args;
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_CONFIG_BUILDER_HPP_
