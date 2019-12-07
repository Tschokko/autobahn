// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <chrono>
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"
#include "msgpack.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "client_config_service.hpp"
#include "message.hpp"
#include "server_handler.hpp"
#include "transport.hpp"

void build_client_configs(
    std::shared_ptr<autobahn::client_config_service> const&
        client_config_service) {
  using boost::asio::ip::make_network_v4;
  using boost::asio::ip::make_network_v6;

  autobahn::client_config config;
  config.set_ipv4_interface_config(make_network_v4("10.18.0.3/24"));
  config.set_ipv6_interface_config(make_network_v6("2001:db8:cafe::3/112"));

  client_config_service->add_or_update_client_config("test", std::move(config));
}

int main() {
  auto context = std::make_shared<zmq::context_t>(1);
  auto transport = std::make_shared<autobahn::zmq_transport>(context);
  auto client_config_service =
      std::make_shared<autobahn::client_config_service>();
  build_client_configs(client_config_service);

  auto handler =
      std::make_shared<autobahn::server_handler>(client_config_service);

  transport->attach(handler);

  transport->bind("ipc:///tmp/autobahn");
  transport->listen();

  return 0;
}
