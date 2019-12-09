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
#include "openvpn/config.hpp"
#include "openvpn/process.hpp"
#include "server_handler.hpp"
#include "transport.hpp"

void build_client_configs(
    std::shared_ptr<autobahn::client_config_service> const&
        client_config_service) {
  using boost::asio::ip::make_network_v4;
  using boost::asio::ip::make_network_v6;

  autobahn::openvpn::client_config config;
  config.set_ipv4_interface_config(make_network_v4("100.127.0.123/22"));
  config.set_ipv6_interface_config(
      make_network_v6("2a03:4000:6:11cd:bbbb::1123/112"));

  client_config_service->add_or_update_client_config("client1",
                                                     std::move(config));
}

autobahn::openvpn::config get_openvpn_config() {
  using autobahn::openvpn::compressions;
  using autobahn::openvpn::config;
  using autobahn::openvpn::make_process;
  using autobahn::openvpn::process;
  using autobahn::openvpn::protocols;
  using autobahn::openvpn::topologies;

  using boost::asio::ip::make_network_v4;
  using boost::asio::ip::make_network_v6;

  config conf;
  conf.set_port(9443);
  conf.set_protocol(protocols::tcp_server);
  conf.set_device("tun");
  conf.set_topology(topologies::subnet);
  conf.set_server_ipv4(make_network_v4("100.127.0.0/22"));
  conf.set_server_ipv6(make_network_v6("2a03:4000:6:11cd:bbbb::/112"));

  conf.set_keep_alive(30, 60);
  conf.enable_ping_timer_remote();
  conf.enable_persist_tun();
  conf.enable_persist_key();

  conf.set_auth("SHA512");
  conf.set_cipher("AES-256-CBC");

  conf.set_compression(compressions::lzo);

  conf.set_certificate_authority_file(
      "/home/tlx3m3j/go/src/github.com/tschokko/autobahn/ssl/ca/ca.crt");
  // c.Set("crl-verify", ca.GetCRLPath())
  conf.set_certificate_file(
      "/home/tlx3m3j/go/src/github.com/tschokko/autobahn/ssl/server/"
      "server.crt");
  conf.set_private_key_file(
      "/home/tlx3m3j/go/src/github.com/tschokko/autobahn/ssl/server/"
      "server.key");
  conf.set_diffie_hellman_file(
      "/home/tlx3m3j/go/src/github.com/tschokko/autobahn/ssl/dh2048.pem");

  conf.enable_tls_server();
  conf.set_tls_authentication_file(
      "/home/tlx3m3j/go/src/github.com/tschokko/autobahn/ssl/ta.key", 0);
  conf.set_minimum_tls_version("1.2");
  conf.set_tls_cipher(
      "TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256:TLS-ECDHE-ECDSA-WITH-AES-128-GCM-"
      "SHA256:TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384:TLS-DHE-RSA-WITH-AES-256-"
      "CBC-SHA256");

  conf.set_value("setenv", "AUTOBAHN_SERVER_ADDRESS ipc:///tmp/autobahn");
  conf.set_value("plugin",
                 "/home/tlx3m3j/src/github.com/tschokko/autobahn-plugin/"
                 "bazel-bin/autobahn/autobahn-plugin.so");

  return conf;
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
  std::thread listening_thread([&] { transport->listen(); });


  auto server_process = std::make_shared<autobahn::openvpn::process>(
      std::move(get_openvpn_config()));
  std::error_code ec;
  server_process->start(ec);

  return 0;
}
