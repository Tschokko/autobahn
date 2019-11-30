#include <iostream>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"

#include "config_builder.hpp"

int main() {
  using namespace autobahn;
  using namespace boost::asio;
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

  return 0;
}
