// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_TRANSPORT_HPP_
#define AUTOBAHN_SRC_TRANSPORT_HPP_

#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "boost/asio.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "message.hpp"

namespace autobahn {

template <class T>
class TransportHandler;

template <class T>
class Transport {
 public:
  typedef autobahn::Transport<T> TransportType;
  typedef std::shared_ptr<autobahn::TransportHandler<T>>
      TransportHandlerPtrType;

  virtual ~Transport() = default;

  virtual void SendMessage(T&& message) = 0;
  virtual void Attach(TransportHandlerPtrType const& handler) = 0;
  virtual void Detach() = 0;
};


template <class T>
class TransportHandler {
 public:
  typedef std::shared_ptr<autobahn::Transport<T>> TransportPtrType;

  virtual ~TransportHandler() = default;

  virtual void OnAttach(TransportPtrType const& transport) = 0;
  virtual void OnDetach() = 0;
  virtual void OnMessage(T&& message) = 0;
};

class ZMQTransport : public Transport<autobahn::Message>,
                     public std::enable_shared_from_this<ZMQTransport> {
 public:
  explicit ZMQTransport(std::shared_ptr<zmq::context_t> const& context)
      : context_(context),
        socket_(*context, zmq::socket_type::pair),
        shutdown_socket_(*context, zmq::socket_type::pair) {
    // Bind to our shutdown socket to receive the shutdown message send by the
    // Shutdown function.
    shutdown_socket_.bind("inproc://shutdown_socket");
  }

  void Bind(std::string const& addr) { socket_.bind(addr); }
  void Connect(std::string const& addr) { socket_.connect(addr); }

  void SendMessage(autobahn::Message&& message) {
    std::cerr << "Send Message: message_type="
              << static_cast<int>(message.message_type())
              << ", subject=" << message.subject() << std::endl;

    zmq::multipart_t msg;
    msg.addstr(EncodeMessageTypeToString(message.message_type()));
    msg.addstr(message.subject());
    msg.addstr(message.data());

    msg.send(socket_);
  }

  void Attach(const TransportHandlerPtrType& handler) {
    if (handler_) {
      throw std::logic_error("handler already attached");
    }
    handler_ = handler;
    handler_->OnAttach(
        std::static_pointer_cast<TransportType>(shared_from_this()));
  }

  void Detach() {
    if (!handler_) {
      throw std::logic_error("no handler attached");
    }
    handler_->OnDetach();
    handler_.reset();
  }

  void Listen() {
    zmq_pollitem_t items[] = {{socket_, 0, ZMQ_POLLIN, 0},
                              {shutdown_socket_, 0, ZMQ_POLLIN, 0}};

    for (;;) {
      if (zmq::poll(items, 2, -1) > 0) {
        if (items[0].revents & ZMQ_POLLIN) {
          zmq::multipart_t msg;
          msg.recv(socket_);

          auto message_type = DecodeMessageTypeString(msg.popstr());
          auto subject = msg.popstr();
          auto data = msg.popstr();

          std::cerr << "Recv Message: message_type="
                    << static_cast<int>(message_type) << ", subject=" << subject
                    << std::endl;

          if (handler_) {
            handler_->OnMessage(
                autobahn::MakeMessage(message_type, subject, data));
          }
        }
        if (items[1].revents & ZMQ_POLLIN) {
          std::cerr << "Shutdown signal received" << std::endl;
          break;
        }
      }
    }

    // Clean properly our ZeroMQ sockets
    socket_.setsockopt(ZMQ_LINGER, 0);
    shutdown_socket_.setsockopt(ZMQ_LINGER, 0);

    socket_.close();
    shutdown_socket_.close();

    shutdown_.set_value(true);
  }

  void Shutdown(size_t timeout) {
    zmq::socket_t socket(*context_, zmq::socket_type::pair);
    socket.connect("inproc://shutdown_socket");
    zmq::message_t shutdown_msg("0", 1);
    socket.send(shutdown_msg, zmq::send_flags::dontwait);

    auto shutdown_future = shutdown_.get_future();

    if (shutdown_future.wait_for(std::chrono::seconds(timeout)) ==
        std::future_status::timeout) {
      throw std::runtime_error("shutdown failed");
    }
  }

 private:
  TransportHandlerPtrType handler_;
  std::shared_ptr<zmq::context_t> context_;
  zmq::socket_t socket_;
  zmq::socket_t shutdown_socket_;
  std::promise<bool> shutdown_;

  static std::string EncodeMessageTypeToString(MessageTypes v) {
    switch (v) {
      case MessageTypes::kRequest:
        return "REQ";
      case MessageTypes::kReply:
        return "REP";
      case MessageTypes::kPublish:
        return "PUB";
    }
  }

  static MessageTypes DecodeMessageTypeString(std::string const& str) {
    if (str == "REQ") return MessageTypes::kRequest;
    if (str == "REP") return MessageTypes::kReply;
    if (str == "PUB") return MessageTypes::kPublish;
    throw std::bad_cast();
  }
};

/*template <class T>
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
};*/

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_TRANSPORT_HPP_
