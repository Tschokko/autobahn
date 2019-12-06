// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_SERVER_HANDLER_HPP_
#define AUTOBAHN_SRC_SERVER_HANDLER_HPP_

#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

#include "message.hpp"
#include "transport.hpp"

namespace autobahn {

class ServerHandler : public autobahn::TransportHandler<autobahn::Message>,
                      public std::enable_shared_from_this<ServerHandler> {
 public:
  void OnAttach(const TransportPtrType& transport) { transport_ = transport; }

  void OnDetach() { transport_.reset(); }

  void OnMessage(autobahn::Message&& message) {
    switch (message.message_type()) {
      case autobahn::MessageTypes::kRequest:
        ProcessRequestMessage(std::move(message));
        break;
      case autobahn::MessageTypes::kReply:
        // log not supported
        break;
      case autobahn::MessageTypes::kPublish:
        // log not supported
        break;
    }
  }

 private:
  TransportPtrType transport_;

  void SendMessage(autobahn::Message&& message) {
    if (!transport_) {
      throw std::logic_error("No transport attached");
    }

    transport_->SendMessage(std::move(message));
  }

  void ProcessRequestMessage(autobahn::Message&& message) {
    if (message.subject() == autobahn::Subjects::kClientConnect) {
      ProcessClientConnectRequest(std::move(message));
    } else if (message.subject() == autobahn::Subjects::kLearnAddress) {
      ProcessLearnAddressRequest(std::move(message));
    }
  }

  void ProcessClientConnectRequest(autobahn::Message&& message) {
    auto request = message.data<autobahn::ClientConnectRequest>();

    auto reply =
        autobahn::MakeClientConnectReply(request.request_id(), true, "config");
    auto reply_msg = autobahn::MakeMessage<autobahn::ClientConnectReply>(
        autobahn::MessageTypes::kReply, autobahn::Subjects::kClientConnect,
        reply);

    SendMessage(std::move(reply_msg));
  }

  void ProcessLearnAddressRequest(autobahn::Message&& message) {
    auto request = message.data<autobahn::LearnAddressRequest>();

    auto reply = autobahn::MakeLearnAddressReply(request.request_id(), true);
    auto reply_msg = autobahn::MakeMessage<autobahn::LearnAddressReply>(
        autobahn::MessageTypes::kReply, autobahn::Subjects::kLearnAddress,
        reply);

    SendMessage(std::move(reply_msg));
  }
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_SERVER_HANDLER_HPP_
