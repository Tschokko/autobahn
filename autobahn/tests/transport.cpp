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

class TransportDummy
    : public autobahn::Transport<autobahn::Message<std::string>>,
      public std::enable_shared_from_this<TransportDummy> {
 public:
  int Connect_called = 0;
  int Disconnect_called = 0;
  int SendMessage_called = 0;
  int Attach_called = 0;
  int Detach_called = 0;
  std::shared_ptr<autobahn::TransportHandler<autobahn::Message<std::string>>>
      handler_;

  virtual void Connect() { Connect_called++; }
  virtual void Disconnect() { Disconnect_called++; }
  virtual bool IsConnected() const { return true; }
  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    SendMessage_called++;
  }
  virtual void
  Attach(const std::shared_ptr<
         autobahn::TransportHandler<autobahn::Message<std::string>>>& handler) {
    Attach_called++;
    handler_ = handler;
    handler_->OnAttach(this->shared_from_this());
  }
  virtual void Detach() {
    Detach_called++;
    handler_->OnDetach();
    handler_.reset();
  }
  virtual bool HasHandler() const { return true; }
};

class TransportRequestClientConnectStub : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    TransportDummy::SendMessage(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::message::RequestClientConnect>();

      // Prepare and send client connect reply
      auto reply = autobahn::message::MakeReplyClientConnect(
          request.request_id(), true, "config");
      auto reply_msg =
          autobahn::MakeMessage<std::string,
                                autobahn::message::ReplyClientConnect>(
              autobahn::MessageType::kReply,
              autobahn::Subjects::kClientConnectSubject, reply);

      handler_->OnMessage(std::move(reply_msg));
    }
  }
};

class TransportRequestClientConnectRequestIDNotInMapStub
    : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    TransportDummy::SendMessage(std::move(message));
    if (handler_) {
      // Get request id from request message
      auto request = message.data<autobahn::message::RequestClientConnect>();

      // Prepare and send client connect reply, but manipulate request id!
      auto reply = autobahn::message::MakeReplyClientConnect(
          request.request_id() + 1, true, "config");
      auto reply_msg =
          autobahn::MakeMessage<std::string,
                                autobahn::message::ReplyClientConnect>(
              autobahn::MessageType::kReply,
              autobahn::Subjects::kClientConnectSubject, reply);

      handler_->OnMessage(std::move(reply_msg));
    }
  }
};

class TransportEventLearnAddressStub : public TransportDummy {
 public:
  std::string SendMessage_subject;
  std::string EventLearnAddress_common_name;
  std::string EventLearnAddress_addr;

  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    TransportDummy::SendMessage(std::move(message));
    auto event = message.data<autobahn::message::EventLearnAddress>();
    SendMessage_subject = message.subject();
    EventLearnAddress_common_name = event.common_name();
    EventLearnAddress_addr = event.addr();
  }
};

/*TEST(plugin_Transport, SendMessage_Called) {
  // Arrange
  auto transport = std::make_shared<TransportRequestClientConnectStub>();

  // Act
  transport->SendMessage(autobahn::MakeMessage<std::string>(
      autobahn::MessageType::kRequest,
      autobahn::Subjects::kClientConnectSubject, "test"));

  // Assert
  ASSERT_EQ(transport->SendMessage_called, 1);
}*/

TEST(PluginHandler, RequestClientConnect_NoTransportException) {
  // Arrange
  auto transport = std::make_shared<TransportRequestClientConnectStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  // We do not attach the handler to raise the expected exception.
  // transport->Attach(handler);

  // Act & Assert
  ASSERT_THROW(handler->RequestClientConnect("test"), std::logic_error);
}

TEST(PluginHandler, RequestClientConnect_AuthorizedClient) {
  // Arrange
  auto transport = std::make_shared<TransportRequestClientConnectStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  // Act
  auto reply = handler->RequestClientConnect("test");

  // Assert
  ASSERT_TRUE(reply.authorized());
}

TEST(PluginHandler, RequestClientConnect_RequestIDNotInMap) {
  // Arrange
  auto transport =
      std::make_shared<TransportRequestClientConnectRequestIDNotInMapStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  // Act
  // auto reply = handler->RequestClientConnect("test");

  // Act & Assert
  ASSERT_THROW(handler->RequestClientConnect("test"), std::logic_error);
}

TEST(PluginHandler, PublishLearnAddress_ValidMessage) {
  // Arrange
  auto transport = std::make_shared<TransportEventLearnAddressStub>();
  auto handler = std::make_shared<autobahn::PluginHandler>();

  transport->Attach(handler);

  // Act
  handler->PublishLearnAddress("test", "10.18.0.3/24");

  // Assert
  ASSERT_EQ(transport->SendMessage_called, 1);
  ASSERT_EQ(transport->SendMessage_subject,
            autobahn::Subjects::kLearnAddressSubject);
  ASSERT_EQ(transport->EventLearnAddress_common_name, "test");
  ASSERT_EQ(transport->EventLearnAddress_addr, "10.18.0.3/24");
}
