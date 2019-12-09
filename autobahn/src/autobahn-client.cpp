// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <iostream>

#include "boost/asio.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"
#include "msgpack.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "message.hpp"
#include "openvpn/config_builder.hpp"
#include "plugin_handler.hpp"
#include "transport.hpp"

namespace autobahn::plugin {

int main() {
  auto context = std::make_shared<zmq::context_t>(1);
  auto transport = std::make_shared<autobahn::zmq_transport>(context);
  auto handler = std::make_shared<autobahn::plugin_handler>();

  transport->attach(handler);

  transport->connect("ipc:///tmp/autobahn");

  std::thread listening_thread([&] { transport->listen(); });

  std::error_code client_connect_ec;
  auto client_connect_reply =
      handler->request_client_connect("test", client_connect_ec);
  if (!client_connect_ec) {
    std::cout << "Request client connect: config="
              << client_connect_reply.config() << std::endl;
  } else {
    std::cerr << client_connect_ec << std::endl;
  }

  std::error_code learn_address_ec;
  auto learn_address_reply =
      handler->request_learn_address(autobahn::learn_address_operations::add,
                                     "10.18.0.3/24", "test", learn_address_ec);
  if (!learn_address_ec) {
    std::cout << "Request learn address: learned="
              << learn_address_reply.learned() << std::endl;
  } else {
    std::cerr << learn_address_ec << std::endl;
  }

  transport->shutdown(3);
  listening_thread.join();

  return 0;
}

}  // namespace autobahn::plugin

int main() { return autobahn::plugin::main(); }

/*openvpn::ConfigBuilder config;

config.SetPort(9443);
config.SetProtocol(openvpn::kProtocolTCPServer);
config.SetDevice("tun");
config.SetTopology(openvpn::kTopologySubnet);
config.SetServerIPv4(ip::make_network_v4("100.127.0.0/22"));
config.SetServerIPv6(ip::make_network_v6("2a03:4000:6:11cd:bbbb::/112"));

config.SetKeepAlive(10, 60);
config.SetPingTimerRemote();
config.SetPersistTun();
config.SetPersistKey();

config.SetAuth("SHA512");
config.SetCipher("AES-256-CBC");
config.SetCompression(openvpn::kCompressionLZO);

config.SetCertificateAuthorityFile("./ssl/ca/ca.crt");
config.SetCertificateFile("./ssl/server/server.crt");
config.SetPrivateKeyFile("./ssl/server/server.key");
config.SetDiffieHellmanFile("./ssl/dh2048.pem");

config.SetTLSServer();
config.SetTLSAuthentication("./ssl/ta.key", 0);
config.SetTLSVersion("1.2");
config.SetTLSCipher(
    "TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256:TLS-ECDHE-ECDSA-WITH-AES-128-GCM-"
    "SHA256:TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384:TLS-DHE-RSA-WITH-AES-256-"
    "CBC-SHA256");

std::cout << "openvpn ";
for (auto const& arg : config.BuildArgs()) std::cout << arg << " ";
std::cout << std::endl;*/
