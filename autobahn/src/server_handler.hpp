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

class ServerHandler
    : public autobahn::TransportHandler<autobahn::Message<std::string>>,
      public std::enable_shared_from_this<ServerHandler> {
 public:
  void OnAttach(
      const std::shared_ptr<
          autobahn::Transport<autobahn::Message<std::string>>>& transport) {
    transport_ = transport;
  }

  void OnDetach() { transport_.reset(); }

  void OnMessage(autobahn::Message<std::string>&& message) {
    switch (message.message_type()) {
      case autobahn::MessageType::kRequest:
        ProcessRequestMessage(std::move(message));
        break;
      case autobahn::MessageType::kPublish:
        ProcessPublishMessage(std::move(message));
        break;
    }
  }

 private:
  std::shared_ptr<autobahn::Transport<autobahn::Message<std::string>>>
      transport_;

  // std::promise<autobahn::message::ReplyClientConnect> client_connect_reply_;
  std::map<uint, std::promise<autobahn::message::ReplyClientConnect>>
      client_connect_replies_;

  void ProcessRequestMessage(autobahn::Message<std::string>&& message) {
    if (message.subject() == autobahn::Subjects::kClientConnectSubject) {
      ProcessClientConnectRequest(std::move(message));
    }
  }

  void ProcessPublishMessage(autobahn::Message<std::string>&& message) {
    if (message.subject() == autobahn::Subjects::kLearnAddressSubject) {
      ProcessLearnAddressEvent(std::move(message));
    }
  }

  void ProcessClientConnectRequest(autobahn::Message<std::string>&& message) {
    if (!transport_) {
      throw std::logic_error("no transport attached");
    }

    auto request = message.data<autobahn::message::RequestClientConnect>();

    auto reply = autobahn::message::MakeReplyClientConnect(request.request_id(),
                                                           true, "config");
    auto reply_msg =
        autobahn::MakeMessage<std::string,
                              autobahn::message::ReplyClientConnect>(
            autobahn::MessageType::kReply,
            autobahn::Subjects::kClientConnectSubject, reply);

    transport_->SendMessage(std::move(reply_msg));
  }

  void ProcessLearnAddressEvent(autobahn::Message<std::string>&& message) {}
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_SERVER_HANDLER_HPP_
