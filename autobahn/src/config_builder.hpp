// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_CONFIG_BUILDER_HPP_
#define AUTOBAHN_SRC_CONFIG_BUILDER_HPP_

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"

namespace autobahn::openvpn {

using boost::asio::ip::network_v4;
using boost::asio::ip::network_v6;

enum Protocols {
  kProtocolUDP = 1,
  kProtocolTCPServer = 2,
  kProtocolTCPClient = 3
};

enum Topologies { kTopologyNet30 = 1, kTopologyP2P = 2, kTopologySubnet = 3 };

enum Compressions { kCompressionLZO = 1, kCompressionLZ4 = 2 };

class ConfigBuilder {
 public:
  void SetFlag(std::string const& flag) {
    if (!HasFlag(flag)) flags_.push_back(flag);
  }

  void SetValue(std::string const& key, std::string const& value) {
    values_[key] = value;
  }

  void SetPort(uint16_t port) { SetValue("port", std::to_string(port)); }
  void SetProtocol(Protocols const& protocol) {
    switch (protocol) {
      case kProtocolUDP:
        SetValue("proto", "udp");
        break;
      case kProtocolTCPServer:
        SetValue("proto", "tcp-server");
        break;
      case kProtocolTCPClient:
        SetValue("proto", "tcp-client");
        break;
    }
  }
  void SetDevice(std::string const& device) { SetValue("dev", device); }
  void SetTopology(Topologies const& topology) {
    switch (topology) {
      case kTopologyNet30:
        SetValue("topology", "net30");
        break;
      case kTopologyP2P:
        SetValue("topology", "p2p");
        break;
      case kTopologySubnet:
        SetValue("topology", "subnet");
        break;
    }
  }
  void SetServerIPv4(network_v4 const& server) {
    std::stringstream sout;
    sout << server.address().to_string() << " " << server.netmask().to_string();
    SetValue("server", sout.str());
  }
  void SetServerIPv6(network_v6 const& server) {
    SetValue("server-ipv6", server.to_string());
  }
  void SetKeepAlive(uint interval, int timeout) {
    std::stringstream sout;
    sout << interval << " " << timeout;
    SetValue("keepalive", sout.str());
  }
  void SetAuth(std::string const& auth) { SetValue("auth", auth); }
  void SetCipher(std::string const& cipher) { SetValue("cipher", cipher); }
  void SetCompression(Compressions const& compression) {
    switch (compression) {
      case kCompressionLZO:
        SetValue("compress", "lzo");
        break;
      case kCompressionLZ4:
        SetValue("compress", "lz4");
        break;
    }
  }
  void SetCertificateAuthorityFile(
      std::string const& certificate_authority_file) {
    SetValue("ca", certificate_authority_file);
  }
  void SetCertificateFile(std::string const& certificate_file) {
    SetValue("cert", certificate_file);
  }
  void SetPrivateKeyFile(std::string const& private_key_file) {
    SetValue("key", private_key_file);
  }
  void SetDiffieHellmanFile(std::string const& diffie_hellman_file) {
    SetValue("dh", diffie_hellman_file);
  }
  void SetTLSAuthentication(std::string const& tls_authentication_file,
                            int direction) {
    SetValue("tls-auth",
             tls_authentication_file + " " + std::to_string(direction));
  }
  void SetTLSVersion(std::string const& tls_version) {
    SetValue("tls-version-min", tls_version);
  }
  void SetTLSCipher(std::string const& tls_cipher) {
    SetValue("tls-cipher", tls_cipher);
  }

  void SetPersistTun() { SetFlag("persist-tun"); }
  void SetPersistKey() { SetFlag("persist-key"); }
  void SetPingTimerRemote() { SetFlag("ping-timer-rem"); }
  void SetTLSServer() { SetFlag("tls-server"); }

  std::vector<std::string> BuildArgs() {
    std::vector<std::string> args;
    for (auto const& arg : flags_) {
      args.push_back("--" + arg);
    }
    for (const auto& [arg, value] : values_) {
      args.push_back("--" + arg + " " + value);
    }
    return args;
  }

 private:
  std::map<std::string, std::string> values_;
  std::vector<std::string> flags_;

  bool HasFlag(std::string const& flag) {
    for (auto const& f : flags_) {
      if (f == flag) return true;
    }
    return false;
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_CONFIG_BUILDER_HPP_
