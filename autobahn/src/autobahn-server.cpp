// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <chrono>
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "msgpack.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "message.hpp"
#include "server_handler.hpp"
#include "transport.hpp"

int main() {
  auto context = std::make_shared<zmq::context_t>(1);
  auto transport = std::make_shared<autobahn::ZMQTransport>(context);
  auto handler = std::make_shared<autobahn::ServerHandler>();

  transport->Attach(handler);

  transport->Bind("ipc:///tmp/autobahn");
  transport->Listen();

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
      auto rep_data =
  autobahn::message::MsgPackEncoding::EncodeToString(rep);

      zmq::multipart_t reply_msg(rep_data);
      reply_msg.send(sock);
    }
  }*/

  return 0;
}
