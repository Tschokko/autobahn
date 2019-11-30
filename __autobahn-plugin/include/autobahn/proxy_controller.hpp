// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_PROXY_CONTROLLER_HPP_
#define AUTOBAHN_PROXY_CONTROLLER_HPP_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "autobahn/client_config.hpp"

namespace autobahn {

class ProxyController {
 public:
  bool Connect(const std::string& addr);

  bool RequestClientConnect(const std::string& common_name,
                            ClientConfig& client_config);
  void PublishAddAddress(const std::string& addr,
                         const std::string& common_name);
  void PublishDeleteAddress(const std::string& addr);
  void PublishClientDisconnect(const std::string& common_name);
};

}  // namespace autobahn

#include "autobahn/proxy_controller.ipp"
#endif  // AUTOBAHN_PROXY_CONTROLLER_HPP_