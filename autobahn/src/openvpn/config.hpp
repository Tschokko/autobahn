// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_OPENVPN_CONFIG_HPP_
#define AUTOBAHN_SRC_OPENVPN_CONFIG_HPP_

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"

namespace autobahn::openvpn {

using boost::asio::ip::network_v4;
using boost::asio::ip::network_v6;

enum protocols { udp = 1, tcp_server = 2, tcp_client = 3 };

enum class topologies { net30 = 1, p2p = 2, subnet = 3 };

enum class compressions { lzo = 1, lz4 = 2 };

class config {
 public:
  void set_flag(std::string const& flag) {
    if (!has_flag(flag)) flags_.push_back(flag);
  }

  void set_value(std::string const& key, std::string const& value) {
    values_[key] = value;
  }

  void set_port(uint16_t port) { set_value("port", std::to_string(port)); }
  void set_protocol(protocols const& protocol) {
    switch (protocol) {
      case protocols::udp:
        set_value("proto", "udp");
        break;
      case protocols::tcp_server:
        set_value("proto", "tcp-server");
        break;
      case protocols::tcp_client:
        set_value("proto", "tcp-client");
        break;
    }
  }
  void set_device(std::string const& device) { set_value("dev", device); }
  void set_topology(topologies const& topology) {
    switch (topology) {
      case topologies::net30:
        set_value("topology", "net30");
        break;
      case topologies::p2p:
        set_value("topology", "p2p");
        break;
      case topologies::subnet:
        set_value("topology", "subnet");
        break;
    }
  }
  void set_server_ipv4(network_v4 const& server) {
    std::stringstream sout;
    sout << server.address().to_string() << " " << server.netmask().to_string();
    set_value("server", sout.str());
  }
  void set_server_ipv6(network_v6 const& server) {
    set_value("server-ipv6", server.to_string());
  }
  void set_keep_alive(uint interval, int timeout) {
    std::stringstream sout;
    sout << interval << " " << timeout;
    set_value("keepalive", sout.str());
  }
  void set_auth(std::string const& auth) { set_value("auth", auth); }
  void set_cipher(std::string const& cipher) { set_value("cipher", cipher); }
  void set_compression(compressions const& compression) {
    switch (compression) {
      case compressions::lzo:
        set_value("compress", "lzo");
        break;
      case compressions::lz4:
        set_value("compress", "lz4");
        break;
    }
  }
  void set_certificate_authority_file(
      std::string const& certificate_authority_file) {
    set_value("ca", certificate_authority_file);
  }
  void set_certificate_file(std::string const& certificate_file) {
    set_value("cert", certificate_file);
  }
  void set_private_key_file(std::string const& private_key_file) {
    set_value("key", private_key_file);
  }
  void set_diffie_hellman_file(std::string const& diffie_hellman_file) {
    set_value("dh", diffie_hellman_file);
  }
  void set_tls_authentication_file(std::string const& tls_authentication_file,
                                   int direction) {
    set_value("tls-auth",
              tls_authentication_file + " " + std::to_string(direction));
  }
  void set_minimum_tls_version(std::string const& tls_version) {
    set_value("tls-version-min", tls_version);
  }
  void set_tls_cipher(std::string const& tls_cipher) {
    set_value("tls-cipher", tls_cipher);
  }

  void enable_persist_tun() { set_flag("persist-tun"); }
  void enable_persist_key() { set_flag("persist-key"); }
  void enable_ping_timer_remote() { set_flag("ping-timer-rem"); }
  void enable_tls_server() { set_flag("tls-server"); }

 private:
  friend class config_builder;

  std::map<std::string, std::string> values_;
  std::vector<std::string> flags_;

  bool has_flag(std::string const& flag) {
    for (auto const& f : flags_) {
      if (f == flag) return true;
    }
    return false;
  }
};

}  // namespace autobahn::openvpn

#endif  // AUTOBAHN_SRC_OPENVPN_CONFIG_HPP_
