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

#include "error_codes.hpp"
#include "message.hpp"
#include "transport.hpp"

namespace autobahn {

class PluginHandler : public autobahn::TransportHandler<autobahn::Message>,
                      public std::enable_shared_from_this<PluginHandler> {
 public:
  autobahn::ClientConnectReply RequestClientConnect(
      std::string const& common_name, std::error_code& ec) {
    // Add a client connect request to our map and use the request_id as a
    // correlation id for the response.
    auto request_id = AddClientConnectRequest();

    // Send request message
    auto request = autobahn::MakeClientConnectRequest(request_id, common_name);
    auto request_msg = autobahn::MakeMessage<autobahn::ClientConnectRequest>(
        autobahn::MessageTypes::kRequest, autobahn::Subjects::kClientConnect,
        request);
    SendMessage(std::move(request_msg));

    // Fetch the client connect reply future and wait for the response. We
    // asume that the OnMessage handler will receive the reply within a second.
    auto client_connect_reply_future =
        client_connect_replies_[request_id].get_future();

    // Wait for the reply or return a timeout error
    if (client_connect_reply_future.wait_for(std::chrono::seconds(1)) ==
        std::future_status::timeout) {
      ec = autobahn::ErrorCodes::RequestTimeout;
      return autobahn::MakeClientConnectReply(request_id, false, "");
    }

    // Get the reply before removing the entry from the map
    auto reply = client_connect_reply_future.get();

    // Remove and free entry from map
    client_connect_replies_.erase(request_id);

    return reply;
  }

  autobahn::LearnAddressReply RequestLearnAddress(
      autobahn::LearnAddressOperations operation, std::string const& address,
      std::string const& common_name, std::error_code& ec) {
    // Add a client connect request to our map and use the request_id as a
    // correlation id for the response.
    auto request_id = AddLearnAddressRequest();

    // Send request message
    auto request = autobahn::MakeLearnAddressRequest(request_id, operation,
                                                     address, common_name);
    auto request_msg = autobahn::MakeMessage<autobahn::LearnAddressRequest>(
        autobahn::MessageTypes::kRequest, autobahn::Subjects::kLearnAddress,
        request);
    SendMessage(std::move(request_msg));

    // Fetch the client connect reply future and wait for the response. We
    // asume that the OnMessage handler will receive the reply within a second.
    auto learn_address_reply_future =
        learn_address_replies_[request_id].get_future();

    // Wait for the reply or return a timeout error
    if (learn_address_reply_future.wait_for(std::chrono::seconds(1)) ==
        std::future_status::timeout) {
      ec = autobahn::ErrorCodes::RequestTimeout;
      return autobahn::MakeLearnAddressReply(request_id, false);
    }

    // Get the reply before removing the entry from the map
    auto reply = learn_address_reply_future.get();

    // Remove and free entry from map
    learn_address_replies_.erase(request_id);

    return reply;
  }

  void OnAttach(TransportPtrType const& transport) { transport_ = transport; }

  void OnDetach() { transport_.reset(); }

  void OnMessage(autobahn::Message&& message) {
    switch (message.message_type()) {
      case autobahn::MessageTypes::kRequest:
        // throw not supported
        break;
      case autobahn::MessageTypes::kReply:
        ProcessReplyMessage(std::move(message));
        break;
      case autobahn::MessageTypes::kPublish:
        // throw not supported
        break;
    }
  }

 private:
  TransportPtrType transport_;

  std::map<int, std::promise<autobahn::ClientConnectReply>>
      client_connect_replies_;
  std::map<int, std::promise<autobahn::LearnAddressReply>>
      learn_address_replies_;

  void SendMessage(autobahn::Message&& message) {
    if (!transport_) {
      throw std::logic_error("No transport attached");
    }

    transport_->SendMessage(std::move(message));
  }

  int AddClientConnectRequest() {
    // Generate a random request id
    std::srand(std::time(nullptr));
    int request_id = std::rand();

    // Create a promise for the request
    client_connect_replies_[request_id] =
        std::promise<autobahn::ClientConnectReply>();

    return request_id;
  }

  int AddLearnAddressRequest() {
    // Generate a random request id
    std::srand(std::time(nullptr));
    int request_id = std::rand();

    // Create a promise for the request
    learn_address_replies_[request_id] =
        std::promise<autobahn::LearnAddressReply>();

    return request_id;
  }

  void ProcessReplyMessage(autobahn::Message&& message) {
    if (message.subject() == autobahn::Subjects::kClientConnect) {
      ProcessClientConnectReply(std::move(message));
    } else if (message.subject() == autobahn::Subjects::kLearnAddress) {
      ProcessLearnAddressReply(std::move(message));
    }
  }

  void ProcessClientConnectReply(autobahn::Message&& message) {
    auto reply = message.data<autobahn::ClientConnectReply>();

    if (client_connect_replies_.find(reply.request_id()) ==
        client_connect_replies_.end()) {
      // TODO(DGL) Write an error to the log! Throwing an exception here, will
      //           kill our OpenVPN process. Bad idea!
      // throw std::logic_error("client connect reply has unknown request id");
    }

    client_connect_replies_[reply.request_id()].set_value(reply);
  }

  void ProcessLearnAddressReply(autobahn::Message&& message) {
    auto reply = message.data<autobahn::LearnAddressReply>();

    if (learn_address_replies_.find(reply.request_id()) ==
        learn_address_replies_.end()) {
      // TODO(DGL) Write an error to the log! Throwing an exception here, will
      //           kill our OpenVPN process. Bad idea!
      // throw std::logic_error("client connect reply has unknown request id");
    }

    learn_address_replies_[reply.request_id()].set_value(reply);
  }
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_
