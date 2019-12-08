// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_CONFIG_BUILDER_HPP_
#define AUTOBAHN_SRC_OPENVPN_CONFIG_BUILDER_HPP_

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "boost/algorithm/string.hpp"

#include "config.hpp"

namespace autobahn::openvpn {

using boost::asio::ip::network_v4;
using boost::asio::ip::network_v6;

class config_builder {
 public:
  static std::vector<std::string> build_args(config const& conf) {
    std::vector<std::string> args;
    for (auto const& arg : conf.flags_) {
      args.push_back("--" + arg);
    }
    for (const auto& [arg, value] : conf.values_) {
      args.push_back("--" + arg + " " + value);
    }
    return args;
  }

  static std::vector<std::string> build_flattened_args(config const& conf) {
    std::vector<std::string> flattened_args;
    auto args = build_args(conf);
    std::stringstream ss;

    for (auto const& arg : args) {
      ss << arg << " ";
    }

    auto str = ss.str();
    boost::algorithm::trim(str);
    boost::algorithm::split(flattened_args, str, boost::algorithm::is_space(),
                            boost::algorithm::token_compress_on);

    return flattened_args;
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_CONFIG_BUILDER_HPP_
