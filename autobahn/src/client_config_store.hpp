// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_CLIENT_CONFIG_STORE_HPP_
#define AUTOBAHN_SRC_CLIENT_CONFIG_STORE_HPP_

#include <map>
#include <memory>
#include <string>

#include "zmq.hpp"

namespace autobahn::openvpn::clientconfig {

class ClientConfigStore {
 public:
  void AddOrUpdate(std::string const& common_name, std::string const& config) {
    store_[common_name] = config;
  }
  void Delete(std::string const& common_name) { store_.erase(common_name); }

 private:
  std::map<std::string, std::string> store_;
};

namespace zmqimpl {
class Reflector {
 public:
  void Run(std::shared_ptr<zmq::context_t> const& context,
           std::string const& addr) {
    socket_ = zmq::socket_t(*context, ZMQ_PUB);
    socket_.bind(addr);
  }

 private:
  zmq::socket_t socket_;
};

}  // namespace zmqimpl

}  // namespace autobahn::openvpn::clientconfig

#endif  // AUTOBAHN_SRC_CLIENT_CONFIG_STORE_HPP_
