// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <chrono>
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "message.hpp"
#include "msgpack.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

int main() {
  boost::asio::io_service io_service;
  boost::asio::local::stream_protocol::endpoint ep("/tmp/autobahn.sock");
  boost::asio::local::stream_protocol::acceptor acceptor(io_service, ep);
  boost::asio::local::stream_protocol::socket socket(io_service);

  acceptor.accept(socket);

  for (;;) {
    boost::asio::streambuf buffer;
    boost::system::error_code ec;
    if (0 == boost::asio::read(socket, buffer,
                               boost::asio::transfer_at_least(1), ec))
      break;

    if (ec && ec != boost::asio::error::eof) {
      std::cerr << "error: " << ec.message() << std::endl;
    } else {
      std::string data(boost::asio::buffer_cast<const char*>(buffer.data()));
      std::cout << "message: " << data << std::endl;
      if (data == "quit") break;
    }
  }

  std::cout << "Fine!" << std::endl;
  /*zmq::context_t ctx(1);
  zmq::socket_t sock(ctx, zmq::socket_type::rep);
  sock.bind("ipc:///tmp/autobahn-1");

  std::cout << "Start listening..." << std::endl;

  while (true) {
    zmq::multipart_t multipart;

    auto ok = multipart.recv(sock);
    if (ok) {
      auto req_subject = multipart.popstr();
      std::cout << "subject = " << req_subject << std::endl;

      // Request
      auto req_data = multipart.popstr();
      auto req = autobahn::message::MsgPackEncoding::DecodeString<
          autobahn::message::RequestClientConnect>(req_data);
      std::cout << "data = " << req.common_name() << std::endl;

      // Reply
      auto rep = autobahn::message::MakeReplyClientConnect(
          true, "ifconfig-push 10.18.0.3 255.255.0.0");
      auto rep_data = autobahn::message::MsgPackEncoding::EncodeToString(rep);

      zmq::multipart_t reply_msg(rep_data);
      reply_msg.send(sock);
    }
  }*/

  return 0;
}
