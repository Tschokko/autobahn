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

class TransportDummy
    : public autobahn::Transport<autobahn::Message<std::string>>,
      public std::enable_shared_from_this<TransportDummy> {
 public:
  int Connect_called = 0;
  int Disconnect_called = 0;
  int SendMessage_called = 0;
  int Attach_called = 0;
  int Detach_called = 0;
  std::shared_ptr<autobahn::TransportHandler<autobahn::Message<std::string>>>
      handler_;

  virtual void Connect() { Connect_called++; }
  virtual void Disconnect() { Disconnect_called++; }
  virtual bool IsConnected() const { return true; }
  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    SendMessage_called++;
  }
  virtual void
  Attach(const std::shared_ptr<
         autobahn::TransportHandler<autobahn::Message<std::string>>>& handler) {
    Attach_called++;
    handler_ = handler;
    handler_->OnAttach(this->shared_from_this());
  }
  virtual void Detach() {
    Detach_called++;
    handler_->OnDetach();
    handler_.reset();
  }
  virtual bool HasHandler() const { return true; }
};

class TransportRequestClientConnectStub : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    std::cerr << "TransportRequestClientConnectStub.SendMessage: "
                 "TransportDummy::SendMessage"
              << std::endl;
    TransportDummy::SendMessage(std::move(message));
    if (handler_) {
      std::cerr << "TransportRequestClientConnectStub.SendMessage: "
                   "handler_->OnMessage"
                << std::endl;
      // Get request id from request message
      auto request = message.data<autobahn::message::RequestClientConnect>();

      std::cerr << "TransportRequestClientConnectStub.SendMessage: "
                   "request_id="
                << request.request_id() << std::endl;

      // Prepare and send client connect reply
      auto reply = autobahn::message::MakeReplyClientConnect(
          request.request_id(), true, "config");
      auto reply_msg =
          autobahn::MakeMessage<std::string,
                                autobahn::message::ReplyClientConnect>(
              autobahn::MessageType::kReply,
              autobahn::Subjects::kClientConnectSubject, reply);

      handler_->OnMessage(std::move(reply_msg));
    }
  }
};

int main() {
  using namespace autobahn;
  using namespace boost::asio;

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

  auto transport = std::make_shared<TransportRequestClientConnectStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);
  auto reply = handler->RequestClientConnect("test");

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
