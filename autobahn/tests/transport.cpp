// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <future>
#include <memory>
#include <string>
#include <tuple>

#include "autobahn/src/message.hpp"
#include "autobahn/src/plugin_handler.hpp"
#include "autobahn/src/transport.hpp"
#include "gtest/gtest.h"

class transport_dummy : public autobahn::transport,
                        public std::enable_shared_from_this<transport_dummy> {
 public:
  int send_message__called = 0;
  int attach__called = 0;
  int detach__called = 0;
  transport_handler_ptr_t handler_;

  virtual void bind(std::string const& addr) {}
  virtual void listen() {}
  virtual void send_message(autobahn::message&& message) {
    send_message__called++;
  }
  virtual void attach(const transport_handler_ptr_t& handler) {
    attach__called++;
    handler_ = handler;
    // handler_->on_attach(this->shared_from_this());
    handler_->on_attach(
        std::static_pointer_cast<transport_t>(shared_from_this()));
  }
  virtual void detach() {
    detach__called++;
    handler_->on_detach();
    handler_.reset();
  }
};

class transport_request_client_connect_stub : public transport_dummy {
 public:
  virtual void send_message(autobahn::message&& message) {
    transport_dummy::send_message(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::client_connect_request>();

      // Prepare and send client connect reply
      auto reply = autobahn::make_client_connect_reply(request.request_id(),
                                                       true, "config");
      auto reply_msg = autobahn::make_message<autobahn::client_connect_reply>(
          autobahn::message_types::reply,
          autobahn::message_subjects::clientconnect, reply);

      handler_->on_message(std::move(reply_msg));
    }
  }
};

class transport_request_client_connect_invalid_request_id_stub
    : public transport_dummy {
 public:
  virtual void send_message(autobahn::message&& message) {
    transport_dummy::send_message(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::client_connect_request>();

      // Prepare and send client connect reply, but manipulate request id!
      auto reply = autobahn::make_client_connect_reply(request.request_id() + 1,
                                                       true, "config");
      auto reply_msg = autobahn::make_message<autobahn::client_connect_reply>(
          autobahn::message_types::reply,
          autobahn::message_subjects::clientconnect, reply);

      handler_->on_message(std::move(reply_msg));
    }
  }
};

class transport_request_learn_address_stub : public transport_dummy {
 public:
  virtual void send_message(autobahn::message&& message) {
    transport_dummy::send_message(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::learn_address_request>();

      // Prepare and send learn address reply
      auto reply =
          autobahn::make_learn_address_reply(request.request_id(), true);
      auto reply_msg = autobahn::make_message<autobahn::learn_address_reply>(
          autobahn::message_types::reply,
          autobahn::message_subjects::learnaddress, reply);

      handler_->on_message(std::move(reply_msg));
    }
  }
};

TEST(plugin_handler, request_client_connect__no_transport_exception) {
  // Arrange
  auto transport = std::make_shared<transport_request_client_connect_stub>();
  auto handler = std::make_shared<autobahn::plugin_handler>();

  // We do not attach the handler to raise the expected exception.
  // transport->attach(handler);

  // Act & Assert
  std::error_code ec;
  ASSERT_THROW(handler->request_client_connect("test", ec), std::logic_error);
}

TEST(plugin_handler, request_client_connect__authorized_true) {
  // Arrange
  auto transport = std::make_shared<transport_request_client_connect_stub>();
  auto handler = std::make_shared<autobahn::plugin_handler>();

  transport->attach(handler);

  // Act
  std::error_code ec;
  auto reply = handler->request_client_connect("test", ec);

  // Assert
  ASSERT_EQ(ec.value(), 0);
  ASSERT_TRUE(reply.valid());
}

TEST(plugin_handler, request_client_connect__timeout_error) {
  // Arrange
  auto transport = std::make_shared<
      transport_request_client_connect_invalid_request_id_stub>();
  auto handler = std::make_shared<autobahn::plugin_handler>();

  transport->attach(handler);

  // Act
  std::error_code ec;
  auto reply = handler->request_client_connect("test", ec);

  // Act & Assert
  ASSERT_EQ(ec, autobahn::error_codes::request_timeout);
}

TEST(plugin_handler, request_learn_addresss__learned_true) {
  // Arrange
  auto transport = std::make_shared<transport_request_learn_address_stub>();
  auto handler = std::make_shared<autobahn::plugin_handler>();

  transport->attach(handler);

  // Act
  std::error_code ec;
  auto reply = handler->request_learn_address(
      autobahn::learn_address_operations::add, "10.18.0.3/24", "test", ec);

  // Assert
  ASSERT_EQ(ec.value(), 0);
  ASSERT_TRUE(reply.learned());
}
