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

class plugin_handler : public autobahn::transport_handler,
                       public std::enable_shared_from_this<plugin_handler> {
 public:
  autobahn::client_connect_reply request_client_connect(
      std::string const& common_name, std::error_code& ec) {
    // Add a client connect request to our map and use the request_id as a
    // correlation id for the response.
    auto request_id = add_client_connect_request();

    // Send request message
    auto request =
        autobahn::make_client_connect_request(request_id, common_name);
    auto request_msg = autobahn::make_message<autobahn::client_connect_request>(
        autobahn::message_types::request,
        autobahn::message_subjects::clientconnect, request);
    send_message(std::move(request_msg));

    // Fetch the client connect reply future and wait for the response. We
    // asume that the on_message handler will receive the reply within a second.
    auto client_connect_reply_future =
        client_connect_replies_[request_id].get_future();

    // Wait for the reply or return a timeout error
    if (client_connect_reply_future.wait_for(std::chrono::seconds(1)) ==
        std::future_status::timeout) {
      ec = autobahn::error_codes::request_timeout;
      return autobahn::make_client_connect_reply(request_id, false, "");
    }

    // Get the reply before removing the entry from the map
    auto reply = client_connect_reply_future.get();

    // Remove and free entry from map
    client_connect_replies_.erase(request_id);

    return reply;
  }

  autobahn::learn_address_reply request_learn_address(
      autobahn::learn_address_operations operation, std::string const& address,
      std::string const& common_name, std::error_code& ec) {
    // Add a client connect request to our map and use the request_id as a
    // correlation id for the response.
    auto request_id = add_learn_address_request();

    // Send request message
    auto request = autobahn::make_learn_address_request(request_id, operation,
                                                        address, common_name);
    auto request_msg = autobahn::make_message<autobahn::learn_address_request>(
        autobahn::message_types::request,
        autobahn::message_subjects::learnaddress, request);
    send_message(std::move(request_msg));

    // Fetch the client connect reply future and wait for the response. We
    // asume that the on_message handler will receive the reply within a second.
    auto learn_address_reply_future =
        learn_address_replies_[request_id].get_future();

    // Wait for the reply or return a timeout error
    if (learn_address_reply_future.wait_for(std::chrono::seconds(1)) ==
        std::future_status::timeout) {
      ec = autobahn::error_codes::request_timeout;
      return autobahn::make_learn_address_reply(request_id, false);
    }

    // Get the reply before removing the entry from the map
    auto reply = learn_address_reply_future.get();

    // Remove and free entry from map
    learn_address_replies_.erase(request_id);

    return reply;
  }

  void on_attach(transport_ptr_t const& transport) { transport_ = transport; }

  void on_detach() { transport_.reset(); }

  void on_message(autobahn::message&& message) {
    switch (message.message_type()) {
      case autobahn::message_types::request:
        // throw not supported
        break;
      case autobahn::message_types::reply:
        process_reply_message(std::move(message));
        break;
      case autobahn::message_types::publish:
        // throw not supported
        break;
    }
  }

 private:
  transport_ptr_t transport_;

  std::map<int, std::promise<autobahn::client_connect_reply>>
      client_connect_replies_;
  std::map<int, std::promise<autobahn::learn_address_reply>>
      learn_address_replies_;

  void send_message(autobahn::message&& message) {
    if (!transport_) {
      throw std::logic_error("No transport attached");
    }

    transport_->send_message(std::move(message));
  }

  int add_client_connect_request() {
    // Generate a random request id
    std::srand(std::time(nullptr));
    int request_id = std::rand();

    // Create a promise for the request
    client_connect_replies_[request_id] =
        std::promise<autobahn::client_connect_reply>();

    return request_id;
  }

  int add_learn_address_request() {
    // Generate a random request id
    std::srand(std::time(nullptr));
    int request_id = std::rand();

    // Create a promise for the request
    learn_address_replies_[request_id] =
        std::promise<autobahn::learn_address_reply>();

    return request_id;
  }

  void process_reply_message(autobahn::message&& message) {
    if (message.subject() == autobahn::message_subjects::clientconnect) {
      process_client_connect_reply(std::move(message));
    } else if (message.subject() == autobahn::message_subjects::learnaddress) {
      process_learn_address_reply(std::move(message));
    }
  }

  void process_client_connect_reply(autobahn::message&& message) {
    auto reply = message.data<autobahn::client_connect_reply>();

    if (client_connect_replies_.find(reply.request_id()) ==
        client_connect_replies_.end()) {
      // TODO(DGL) Write an error to the log! Throwing an exception here, will
      //           kill our OpenVPN process. Bad idea!
      // throw std::logic_error("client connect reply has unknown request id");
    }

    // Since the reply isn't processed further we move it, to avoid copying.
    client_connect_replies_[reply.request_id()].set_value(std::move(reply));
  }

  void process_learn_address_reply(autobahn::message&& message) {
    auto reply = message.data<autobahn::learn_address_reply>();

    if (learn_address_replies_.find(reply.request_id()) ==
        learn_address_replies_.end()) {
      // TODO(DGL) Write an error to the log! Throwing an exception here, will
      //           kill our OpenVPN process. Bad idea!
      // throw std::logic_error("client connect reply has unknown request id");
    }

    // Since the reply isn't processed further we move it, to avoid copying.
    learn_address_replies_[reply.request_id()].set_value(std::move(reply));
  }
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_PLUGIN_HANDLER_HPP_
