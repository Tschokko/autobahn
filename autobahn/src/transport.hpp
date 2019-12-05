// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_TRANSPORT_HPP_
#define AUTOBAHN_SRC_TRANSPORT_HPP_

#include <memory>
#include <string>
#include <thread>

#include "boost/asio.hpp"

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

template <class T>
class UnixDomainSocketTransport
    : public Transport,
      public std::enable_shared_from_this<UnixDomainSocketTransport> {
 public:
  explicit UnixDomainSocketTransport(boost::asio::io_service const& io_service,
                                     std::string const& addr)
      : endpoint_(addr),
        acceptor_(io_service, endpoint_),
        socket_(io_service) {}

  void Listen() {
    acceptor.accept(socket_);

    for (;;) {
      boost::asio::streambuf buffer;
      boost::system::error_code ec;
      if (0 == boost::asio::read(socket, buffer,
                                 boost::asio::transfer_at_least(1), ec))
        break;

      if (ec && ec != boost::asio::error::eof) {
        std::cerr << "error: " << ec.message() << std::endl;
      } else {
        std::string data(boost::asio::buffer_cast<const char*>(buffer.data()));

        // std::cout << "message: " << data << std::endl;
        // if (data == "quit") break;
      }
    }
  }
  virtual void Connect() {}
  virtual void Disconnect() {}
  virtual bool IsConnected() const {}
  virtual void SendMessage(T&& message) {}
  virtual void Attach(const std::shared_ptr<TransportHandler<T>>& handler) {
    handler_ = handler;
    handler_->OnAttach(this->shared_from_this());
  }
  virtual void Detach() {
    handler_->OnDetach();
    handler_.reset();
  }
  virtual bool HasHandler() const {}

 private:
  std::shared_ptr<autobahn::TransportHandler> handler_;
  // boost::asio::io_service io_service_;
  boost::asio::local::stream_protocol::endpoint endpoint_;
  boost::asio::local::stream_protocol::acceptor acceptor_;
  boost::asio::local::stream_protocol::socket socket_;
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_TRANSPORT_HPP_
