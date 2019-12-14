// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_BROKER_HPP_
#define AUTOBAHN_SRC_BROKER_HPP_

#include <iostream>
#include <memory>
#include <sstream>

#include "nats.h"

namespace autobahn {

class broker {
 public:
  bool connect(std::string const& url) {
    natsConnection* conn;
    auto status = natsConnection_ConnectTo(&conn, url.c_str());
    if (status != NATS_OK) {
      return false;
    }

    conn_ =
        std::unique_ptr<natsConnection, nats_connection_deleter_functor>(conn);

    return true;
  }

  void publish_client_connect(std::string const& common_name) {
    auto status = natsConnection_PublishString(
        conn_.get(), "autobahn.clientconnect", common_name.c_str());
    if (status != NATS_OK) {
      throw std::runtime_error("failed to publish message");
    }
  }

  void publish_client_disconnect(std::string const& common_name) {
    auto status = natsConnection_PublishString(
        conn_.get(), "autobahn.clientdisconnect", common_name.c_str());
    if (status != NATS_OK) {
      throw std::runtime_error("failed to publish message");
    }
  }

  void publish_learn_address(std::string const& operation,
                             std::string const& address,
                             std::string const& common_name) {
    std::stringstream ss;
    ss << operation << ";" << address << ";" << common_name;
    auto status = natsConnection_PublishString(
        conn_.get(), "autobahn.learnaddress", ss.str().c_str());
    if (status != NATS_OK) {
      throw std::runtime_error("failed to publish message");
    }
  }

 private:
  // For our nats connection pointer we need a proper customer deleter
  struct nats_connection_deleter_functor {
    void operator()(natsConnection* conn) {
      std::cout << "Calling natsConnection_Destroy" << std::endl;
      natsConnection_Destroy(conn);
    }
  };

  std::unique_ptr<natsConnection, nats_connection_deleter_functor> conn_;
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_BROKER_HPP_
