// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <iostream>

#include "boost/asio.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "boost/asio/ip/network_v6.hpp"
#include "msgpack.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "config_builder.hpp"
#include "message.hpp"
#include "plugin_handler.hpp"
#include "transport.hpp"

namespace autobahn::plugin {

/*class MessageHandler {
 public:
  explicit MessageHandler(std::shared_ptr<zmq::context_t> context)
      : socket_(*context, zmq::socket_type::req) {}

  void Connect(std::string const& addr) {
    socket_.connect(addr);
    assert(socket_.connected());
    std::cout << " connected=" << socket_.connected() << std::endl;
  }
  void Listen() {}

  bool IsConnected() const { return socket_.connected(); }

  std::tuple<bool, std::string> RequestClientConnect(
      std::string const& common_name) {
    if (!IsConnected()) {
      std::cout << "not connected" << std::endl;
      throw std::logic_error("socket not connected");
    }

    // Request
    auto req = message::MakeRequestClientConnect("client1");
    auto req_data = message::MsgPackEncoding::EncodeToString(req);

    // Messaging
    zmq::multipart_t multipart;
    multipart.addstr("autobahn.openvpn.client-connect");
    multipart.addstr(req_data);
    multipart.send(socket_, static_cast<int>(zmq::send_flags::dontwait));

    // Response
    zmq::multipart_t recv_msg;
    auto ok = recv_msg.recv(socket_);
    if (ok) {
      auto rep_data = recv_msg.popstr();
      auto rep =
          message::MsgPackEncoding::DecodeString<message::ReplyClientConnect>(
              rep_data);
      // std::cout << "data = [" << rep.authorized() << "] [" << rep.config()
      //          << "]" << std::endl;
      return std::make_tuple(rep.authorized(), rep.config());
    }
  }

 private:
  zmq::socket_t socket_;
};*/

int main() {
  auto context = std::make_shared<zmq::context_t>(1);
  auto transport = std::make_shared<autobahn::ZMQTransport>(context);
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  transport->Connect("ipc:///tmp/autobahn");

  std::thread listening_thread([&] { transport->Listen(); });

  std::error_code ec;
  auto reply = handler->RequestClientConnect("test", ec);
  if (!ec) {
    std::cout << "Request client connect: config=" << reply.config()
              << std::endl;
  } else {
    std::cerr << ec << std::endl;
  }

  std::error_code learn_address_ec;
  auto learn_address_reply = handler->RequestLearnAddress(autobahn::LearnAddressOperations::kAdd, "10.18.0.3/24", "test", ec);
  if (!learn_address_ec) {
    std::cout << "Request learn address: learned=" << learn_address_reply.learned()
              << std::endl;
  } else {
    std::cerr << learn_address_ec << std::endl;
  }

  // handler->PublishLearnAddress("test", "10.18.0.3/24");

  transport->Shutdown(3);

  listening_thread.join();

  // using namespace autobahn;
  // using namespace boost::asio;

  /*zmq::context_t ctx(1);
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
  }*/

  /*auto context = std::make_shared<zmq::context_t>(1);
  auto handler = std::make_shared<MessageHandler>(context);

  handler->Connect("ipc:///tmp/autobahn-2");
  auto rv = handler->RequestClientConnect("client1");
  std::cout << "data = [" << std::get<0>(rv) << "] [" << std::get<1>(rv) << "]"
            << std::endl;*/

  // ---------
  // BOOST unix domain sockets
  // ---------
  /* boost::asio::io_service io_service;
  boost::asio::local::stream_protocol::socket socket(io_service);
  boost::asio::local::stream_protocol::endpoint ep("/tmp/autobahn.sock");
  socket.connect(ep);

  std::string line;
  while (std::getline(std::cin, line)) {
    boost::system::error_code ec;
    if (0 == boost::asio::write(socket, boost::asio::buffer(line), ec)) break;

    if (ec) {
      std::cerr << ec.message() << std::endl;
    } else {
      std::cout << "send ok: " << line << std::endl;
    }
  }*/

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

// std::cout << "openvpn ";
// for (auto const& arg : config.BuildArgs()) std::cout << arg << " ";
// std::cout << std::endl;*/

/*

template <class T>
class PluginHandler : public TransportHandler,
                      public std::enable_shared_from_this<PluginHandler> {
 public:
  virtual void OnAttach(const std::shared_ptr<Transport>& transport) {
    //if (transport_) {
    //  throw std::logic_error(ErrorMessages::kSessionTransportAlreadyAttached);
    //}
    transport_ = transport;
  }
  virtual void OnDetach() {
    //if (!transport_) {
    //  throw std::logic_error(ErrorMessages::kSessionTransportAlreadyDetached);
    //}
    transport_.reset();
  }
  virtual void OnMessage(T&& message) = 0;

 private:
  std::shared_ptr<Transport> transport_;

  std::thread Spawn() {}
};
*/
