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

#include "broker.hpp"
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

  autobahn::openvpn::client_config config2;
  config2.set_ipv4_interface_config(make_network_v4("100.127.0.124/22"));
  config2.set_ipv6_interface_config(
      make_network_v6("2a03:4000:6:11cd:bbbb::1124/112"));

  client_config_service->add_or_update_client_config(
      "9387c5dd-2810-471f-96f3-8e22b50b01d6", std::move(config2));

  autobahn::openvpn::client_config config3;
  config3.set_ipv4_interface_config(make_network_v4("100.127.0.125/22"));
  config3.set_ipv6_interface_config(
      make_network_v6("2a03:4000:6:11cd:bbbb::1125/112"));

  client_config_service->add_or_update_client_config(
      "6bb909bb-6cc6-4312-81ed-8d1d91d41f16", std::move(config3));
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

  conf.set_certificate_authority_file("./ssl/root-ca.crt");
  // c.Set("crl-verify", ca.GetCRLPath())
  conf.set_certificate_file("./ssl/server1.crt");
  conf.set_private_key_file("./ssl/server1.key");
  conf.set_diffie_hellman_file("./ssl/dh2048.pem");

  conf.enable_tls_server();
  conf.set_tls_authentication_file("./ssl/ta.key", 0);
  conf.set_minimum_tls_version("1.2");
  conf.set_tls_cipher(
      "TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256:TLS-ECDHE-ECDSA-WITH-AES-128-GCM-"
      "SHA256:TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384:TLS-DHE-RSA-WITH-AES-256-"
      "CBC-SHA256");

  conf.set_value("setenv", "AUTOBAHN_SERVER_ADDRESS ipc:///tmp/autobahn");
  conf.set_value("plugin", "./bazel-bin/autobahn/autobahn-plugin.so");

  return conf;
}

class server {
 public:
  server() {
    context_ = std::make_shared<zmq::context_t>(1);
    transport_ = std::make_shared<autobahn::zmq_transport>(context_);
    client_config_service_ =
        std::make_shared<autobahn::client_config_service>();
    broker_ = std::make_shared<autobahn::broker>();
    build_client_configs(client_config_service_);
    handler_ = std::make_shared<autobahn::server_handler>(
        client_config_service_, broker_);
    server_process_ = std::make_shared<autobahn::openvpn::process>(
        std::move(get_openvpn_config()));
  }

  void run() {
    broker_->connect("nats://localhost:4222");

    transport_->attach(handler_);
    transport_->bind("ipc:///tmp/autobahn");
    std::thread listening_thread([&] { transport_->listen(); });

    std::thread server_thread([&] {
      std::error_code ec;
      server_process_->run(ec);
    });
    server_thread.join();
    std::cout << "server_thread finished" << std::endl;

    transport_->shutdown(10);
    listening_thread.join();
    std::cout << "listening_thread finished" << std::endl;
  }

  void shutdown(int) {
    std::cout << "SIG RECEIVED----------" << std::endl;
    server_process_->shutdown();
  }

 private:
  std::shared_ptr<zmq::context_t> context_;
  std::shared_ptr<autobahn::zmq_transport> transport_;
  std::shared_ptr<autobahn::client_config_service> client_config_service_;
  std::shared_ptr<autobahn::broker> broker_;
  std::shared_ptr<autobahn::server_handler> handler_;
  std::shared_ptr<autobahn::openvpn::process> server_process_;
};

// Signal handler
std::function<void(int)> signal_handler_wrapper;
void signal_handler_callback(int sig) { signal_handler_wrapper(sig); }

int main() {
  server srv;

  // Install our signal handler
  signal_handler_wrapper =
      std::bind(&server::shutdown, &srv, std::placeholders::_1);
  std::signal(SIGINT, signal_handler_callback);
  std::signal(SIGTERM, signal_handler_callback);

  // Run our server
  srv.run();

  return 0;
}
