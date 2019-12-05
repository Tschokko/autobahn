// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_
#define AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_

#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

#include "message.hpp"
#include "transport.hpp"

namespace autobahn {

class TimeoutError : public std::exception {
  const char* what() const throw() { return "Request timeout"; }
};

class PluginHandler : public autobahn::TransportHandler<autobahn::ZMQMessage>,
                      public std::enable_shared_from_this<PluginHandler> {
 public:
  autobahn::ClientConnectReqly RequestClientConnect(
      std::string const& common_name) {
    if (!transport_) {
      throw std::logic_error("No transport attached");
    }

    auto request_id = OpenClientConnectRequest();

    // Send request message
    auto request = autobahn::MakeClientConnectRequest(request_id, common_name);
    auto request_msg = autobahn::MakeZMQMessage<autobahn::ClientConnectRequest>(
        autobahn::MessageTypes::kRequest,
        autobahn::Subjects::kClientConnectSubject, request);
    transport_->SendMessage(std::move(request_msg));

    // Fetch the client connect reply future and wait for the response. We
    // asume that the OnMessage handler will receive the reply within a second.
    auto client_connect_reply_future =
        client_connect_replies_[request_id].get_future();

    if (client_connect_reply_future.wait_for(std::chrono::seconds(1)) ==
        std::future_status::timeout) {
      throw TimeoutError();
    }

    return client_connect_reply_future.get();
  }

  /*void PublishLearnAddress(std::string const& common_name,
                           std::string const& addr) {
    // Send event message
    auto event = autobahn::message::MakeEventLearnAddress(common_name, addr);
    auto event_msg =
        autobahn::MakeMessage<std::string,
                              autobahn::message::EventLearnAddress>(
            autobahn::MessageType::kPublish,
            autobahn::Subjects::kLearnAddressSubject, event);
    transport_->SendMessage(std::move(event_msg));
  }*/

  void OnAttach(TransportPtr const& transport) { transport_ = transport; }

  void OnDetach() { transport_.reset(); }

  void OnMessage(autobahn::ZMQMessage&& message) {
    switch (message.message_type()) {
      case autobahn::MessageTypes::kReply:
        ProcessReplyMessage(std::move(message));
        break;
    }
  }

 private:
  // std::shared_ptr<autobahn::Transport<autobahn::Message<std::string>>>
  TransportPtr transport_;

  // std::promise<autobahn::message::ReplyClientConnect> client_connect_reply_;
  std::map<int, std::promise<autobahn::ClientConnectReqly>>
      client_connect_replies_;

  int OpenClientConnectRequest() {
    // Generate a random request id
    std::srand(std::time(nullptr));
    int request_id = std::rand();

    // Create a promise for the request
    client_connect_replies_[request_id] =
        std::promise<autobahn::ClientConnectReqly>();

    return request_id;
  }

  void ProcessReplyMessage(autobahn::ZMQMessage&& message) {
    if (message.subject() == autobahn::Subjects::kClientConnectSubject) {
      ProcessClientConnectReply(std::move(message));
    }
  }

  void ProcessClientConnectReply(autobahn::ZMQMessage&& message) {
    auto reply = message.data<autobahn::ClientConnectReqly>();

    if (client_connect_replies_.find(reply.request_id()) ==
        client_connect_replies_.end()) {
      throw std::logic_error("client connect reply has unknown request id");
    }

    client_connect_replies_[reply.request_id()].set_value(reply);
  }
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_
