// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_PROCESS_HPP_
#define AUTOBAHN_SRC_OPENVPN_PROCESS_HPP_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "boost/asio.hpp"
#include "boost/process.hpp"

#include "config.hpp"
#include "config_builder.hpp"

namespace autobahn::openvpn {

class process {
 public:
  explicit process(config&& config) : config_(config) {}
  explicit process(config const& config) : config_(config) {}

  void run(std::error_code& ec) {
    boost::process::ipstream is;
    auto args = config_builder::build_flattened_args(config_);

    child_ =
        boost::process::child(boost::process::search_path("openvpn"),
                              std::move(args), boost::process::std_in.close(),
                              boost::process::std_err > boost::process::null,
                              boost::process::std_out > is);

    std::cout << "Starting openvpn..." << std::endl;

    std::string line;
    while (child_.running() && std::getline(is, line) && !line.empty())
      std::cout << line << std::endl;

    child_.wait();
  }

  void shutdown() {
    if (::kill(child_.id(), SIGTERM) == -1) {
      std::cout << "failed to SIGTERM" << std::endl;
      child_.terminate();
    }
  }

  void terminate() { child_.terminate(); }

 private:
  config config_;
  boost::process::child child_;
};

process make_process(config&& conf) { return process(std::move(conf)); }

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_PROCESS_HPP_
