// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include <memory>
#include <string>

#include "autobahn/src/message.hpp"
#include "autobahn/src/transport.hpp"
#include "gtest/gtest.h"


class TransportDummy
    : public autobahn::Transport<autobahn::Message<std::string>> {
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
  }
  virtual void Detach() {
    Detach_called++;
    handler_.reset();
  }
  virtual bool HasHandler() const { return true; }
};

class TransportRequestClientConnectStub : public TransportDummy {
 public:
  virtual void SendMessage(autobahn::Message<std::string>&& message) {
    TransportDummy::SendMessage(std::move(message));
  }
};

TEST(plugin_Transport, SendMessage_Called) {
  // Arrange
  auto transport = std::make_shared<TransportRequestClientConnectStub>();

  // Act
  transport->SendMessage(autobahn::MakeMessage<std::string>(
      autobahn::MessageType::kRequest, "clientconnect", "test"));

  // Assert
  ASSERT_EQ(transport->SendMessage_called, 1);
}
