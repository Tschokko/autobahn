// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_
#define AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_

#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

#include "message.hpp"
#include "transport.hpp"

namespace autobahn {

class TimeoutError : public std::exception {
  const char* what() const throw() { return "Request timeout"; }
};

class PluginHandler
    : public autobahn::TransportHandler<autobahn::Message<std::string>>,
      public std::enable_shared_from_this<PluginHandler> {
 public:
  autobahn::message::ReplyClientConnect RequestClientConnect(
      std::string const& common_name) {
    if (!transport_) {
      throw std::logic_error("No transport attached");
    }

    auto request_id = OpenClientConnectRequest();

    // Send request message
    auto request =
        autobahn::message::MakeRequestClientConnect(request_id, common_name);
    auto request_msg =
        autobahn::MakeMessage<std::string,
                              autobahn::message::RequestClientConnect>(
            autobahn::MessageType::kRequest,
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

  void PublishLearnAddress(std::string const& common_name,
                           std::string const& addr) {
    // Send event message
    auto event = autobahn::message::MakeEventLearnAddress(common_name, addr);
    auto event_msg =
        autobahn::MakeMessage<std::string,
                              autobahn::message::EventLearnAddress>(
            autobahn::MessageType::kPublish,
            autobahn::Subjects::kLearnAddressSubject, event);
    transport_->SendMessage(std::move(event_msg));
  }

  void OnAttach(
      const std::shared_ptr<
          autobahn::Transport<autobahn::Message<std::string>>>& transport) {
    transport_ = transport;
  }

  void OnDetach() { transport_.reset(); }

  void OnMessage(autobahn::Message<std::string>&& message) {
    switch (message.message_type()) {
      case autobahn::MessageType::kReply:
        ProcessReplyMessage(std::move(message));
        break;
    }
  }

 private:
  std::shared_ptr<autobahn::Transport<autobahn::Message<std::string>>>
      transport_;

  // std::promise<autobahn::message::ReplyClientConnect> client_connect_reply_;
  std::map<uint, std::promise<autobahn::message::ReplyClientConnect>>
      client_connect_replies_;

  int OpenClientConnectRequest() {
    // Generate a random request id
    std::srand(std::time(nullptr));
    int request_id = std::rand();

    // Create a promise for the request
    client_connect_replies_[request_id] =
        std::promise<autobahn::message::ReplyClientConnect>();

    return request_id;
  }

  void ProcessReplyMessage(autobahn::Message<std::string>&& message) {
    if (message.subject() == autobahn::Subjects::kClientConnectSubject) {
      ProcessClientConnectReply(std::move(message));
    }
  }

  void ProcessClientConnectReply(autobahn::Message<std::string>&& message) {
    auto reply = message.data<autobahn::message::ReplyClientConnect>();

    if (client_connect_replies_.find(reply.request_id()) ==
        client_connect_replies_.end()) {
      throw std::logic_error("client connect reply has unknown request id");
    }

    client_connect_replies_[reply.request_id()].set_value(reply);
  }
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_