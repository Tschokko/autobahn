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

#include "broker.hpp"
#include "client_config_service.hpp"
#include "message.hpp"
#include "transport.hpp"

namespace autobahn {

class server_handler : public autobahn::transport_handler,
                       public std::enable_shared_from_this<server_handler> {
 public:
  server_handler(std::shared_ptr<autobahn::client_config_service> const&
                     client_config_service,
                 std::shared_ptr<autobahn::broker> const& broker)
      : client_config_service_(client_config_service), broker_(broker) {}

  void on_attach(const transport_ptr_t& transport) { transport_ = transport; }

  void on_detach() { transport_.reset(); }

  void on_message(autobahn::message&& message) {
    switch (message.message_type()) {
      case autobahn::message_types::request:
        process_request_message(std::move(message));
        break;
      case autobahn::message_types::reply:
        // log not supported
        break;
      case autobahn::message_types::publish:
        // log not supported
        break;
    }
  }

 private:
  std::shared_ptr<autobahn::client_config_service> client_config_service_;
  std::shared_ptr<autobahn::broker> broker_;
  transport_ptr_t transport_;

  void send_message(autobahn::message&& message) {
    if (!transport_) {
      throw std::logic_error(
          "handler send message failed: no transport attached");
    }

    transport_->send_message(std::move(message));
  }

  void process_request_message(autobahn::message&& message) {
    if (message.subject() == autobahn::message_subjects::clientconnect) {
      process_client_connect_request(std::move(message));
    } else if (message.subject() == autobahn::message_subjects::learnaddress) {
      process_learn_address_request(std::move(message));
    }
  }

  void process_client_connect_request(autobahn::message&& message) {
    auto request = message.data<autobahn::client_connect_request>();

    auto const& [valid, config] =
        client_config_service_->get_client_config(request.common_name());

    auto reply = autobahn::make_client_connect_reply(request.request_id(),
                                                     valid, config);
    auto reply_msg = autobahn::make_message<autobahn::client_connect_reply>(
        autobahn::message_types::reply,
        autobahn::message_subjects::clientconnect, reply);

    send_message(std::move(reply_msg));

    // Notify observers that a new client has connected successfully
    /*if (valid) {
      broker_->publish_client_connect(request.common_name());
    }*/
  }

  void process_learn_address_request(autobahn::message&& message) {
    auto request = message.data<autobahn::learn_address_request>();

    auto reply = autobahn::make_learn_address_reply(request.request_id(), true);
    auto reply_msg = autobahn::make_message<autobahn::learn_address_reply>(
        autobahn::message_types::reply,
        autobahn::message_subjects::learnaddress, reply);

    send_message(std::move(reply_msg));

    // Notify observers that we learned an address
    /*broker_->publish_learn_address(
        learn_address_operation_to_string(request.operation()),
        request.address(), request.common_name());*/
  }
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_SERVER_HANDLER_HPP_
