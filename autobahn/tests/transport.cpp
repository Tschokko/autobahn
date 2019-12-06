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

class TransportDummy : public autobahn::Transport<autobahn::Message>,
                       public std::enable_shared_from_this<TransportDummy> {
 public:
  int SendMessage_called = 0;
  int Attach_called = 0;
  int Detach_called = 0;
  TransportHandlerPtrType handler_;

  virtual void SendMessage(autobahn::Message&& message) {
    SendMessage_called++;
  }
  virtual void Attach(const TransportHandlerPtrType& handler) {
    Attach_called++;
    handler_ = handler;
    handler_->OnAttach(this->shared_from_this());
  }
  virtual void Detach() {
    Detach_called++;
    handler_->OnDetach();
    handler_.reset();
  }
};

class TransportRequestClientConnectStub : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message&& message) {
    TransportDummy::SendMessage(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::ClientConnectRequest>();

      // Prepare and send client connect reply
      auto reply = autobahn::MakeClientConnectReply(request.request_id(), true,
                                                    "config");
      auto reply_msg = autobahn::MakeMessage<autobahn::ClientConnectReply>(
          autobahn::MessageTypes::kReply, autobahn::Subjects::kClientConnect,
          reply);

      handler_->OnMessage(std::move(reply_msg));
    }
  }
};

class TransportRequestClientConnectRequestIDNotInMapStub
    : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message&& message) {
    TransportDummy::SendMessage(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::ClientConnectRequest>();

      // Prepare and send client connect reply, but manipulate request id!
      auto reply = autobahn::MakeClientConnectReply(request.request_id() + 1,
                                                    true, "config");
      auto reply_msg = autobahn::MakeMessage<autobahn::ClientConnectReply>(
          autobahn::MessageTypes::kReply, autobahn::Subjects::kClientConnect,
          reply);

      handler_->OnMessage(std::move(reply_msg));
    }
  }
};

class TransportRequestLearnAddressStub : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message&& message) {
    TransportDummy::SendMessage(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::LearnAddressRequest>();

      // Prepare and send learn address reply
      auto reply = autobahn::MakeLearnAddressReply(request.request_id(), true);
      auto reply_msg = autobahn::MakeMessage<autobahn::LearnAddressReply>(
          autobahn::MessageTypes::kReply, autobahn::Subjects::kLearnAddress,
          reply);

      handler_->OnMessage(std::move(reply_msg));
    }
  }
};

TEST(PluginHandler, RequestClientConnect_NoTransportException) {
  // Arrange
  auto transport = std::make_shared<TransportRequestClientConnectStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  // We do not attach the handler to raise the expected exception.
  // transport->Attach(handler);

  // Act & Assert
  std::error_code ec;
  ASSERT_THROW(handler->RequestClientConnect("test", ec), std::logic_error);
}

TEST(PluginHandler, RequestClientConnect_AuthorizedTrue) {
  // Arrange
  auto transport = std::make_shared<TransportRequestClientConnectStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  // Act
  std::error_code ec;
  auto reply = handler->RequestClientConnect("test", ec);

  // Assert
  ASSERT_EQ(ec.value(), 0);
  ASSERT_TRUE(reply.authorized());
}

TEST(PluginHandler, RequestClientConnect_Timeout) {
  // Arrange
  auto transport =
      std::make_shared<TransportRequestClientConnectRequestIDNotInMapStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  // Act
  std::error_code ec;
  auto reply = handler->RequestClientConnect("test", ec);

  // Act & Assert
  ASSERT_EQ(ec, autobahn::ErrorCodes::RequestTimeout);
}

TEST(PluginHandler, RequestLearnAddresss_LearnedTrue) {
  // Arrange
  auto transport = std::make_shared<TransportRequestLearnAddressStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  // Act
  std::error_code ec;
  auto reply = handler->RequestLearnAddress(
      autobahn::LearnAddressOperations::kAdd, "10.18.0.3/24", "test", ec);

  // Assert
  ASSERT_EQ(ec.value(), 0);
  ASSERT_TRUE(reply.learned());
}
