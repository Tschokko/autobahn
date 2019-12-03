// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_TRANSPORT_HPP_
#define AUTOBAHN_SRC_TRANSPORT_HPP_

#include <memory>
#include <string>
#include <thread>

namespace autobahn {

template <class T>
class TransportHandler;

template <class T>
class Transport {
 public:
  virtual void Connect() = 0;
  virtual void Disconnect() = 0;
  virtual bool IsConnected() const = 0;
  virtual void SendMessage(T&& message) = 0;
  virtual void Attach(const std::shared_ptr<TransportHandler<T>>& handler) = 0;
  virtual void Detach() = 0;
  virtual bool HasHandler() const = 0;
};

template <class T>
class TransportHandler {
 public:
  virtual ~TransportHandler() = default;

  virtual void OnAttach(const std::shared_ptr<Transport<T>>& transport) = 0;
  virtual void OnDetach() = 0;
  virtual void OnMessage(T&& message) = 0;
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_TRANSPORT_HPP_
