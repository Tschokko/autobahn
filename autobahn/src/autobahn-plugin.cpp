// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <iostream>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"
#include "msgpack.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "config_builder.hpp"
#include "message.hpp"

namespace autobahn {
int main() {
  using namespace autobahn;
  using namespace boost::asio;

  openvpn::ConfigBuilder config;

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
  std::cout << std::endl;

  zmq::context_t ctx(1);
  zmq::socket_t sock(ctx, zmq::socket_type::req);
  sock.connect("ipc:///tmp/autobahn-1");

  // Request
  auto req = message::MakeRequestClientConnect("client1");
  auto req_data = message::MsgPackEncoding::EncodeToString(req);

  // Messaging
  zmq::multipart_t multipart;
  multipart.addstr("autobahn.openvpn.client-connect");
  multipart.addstr(req_data);
  multipart.send(sock);

  // Response
  zmq::multipart_t recv_msg;
  auto ok = recv_msg.recv(sock);
  if (ok) {
    auto rep_data = recv_msg.popstr();
    auto rep =
        message::MsgPackEncoding::DecodeString<message::ReplyClientConnect>(
            rep_data);
    std::cout << "data = [" << rep.authorized() << "] [" << rep.config() << "]"
              << std::endl;
  }

  return 0;
}
}  // namespace autobahn

int main() { return autobahn::main(); }
