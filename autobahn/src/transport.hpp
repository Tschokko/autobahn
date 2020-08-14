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

// TODO(DGL): The transport is responsible for the proper message encoding.
//            That's why the transport implementation needs the encoding spec.
//            A transport handler encodes / decodes messages thru a templated
//            function.
// template <class T>
class transport_handler;

// template <class T>
class transport {
 public:
  typedef autobahn::transport transport_t;
  typedef std::shared_ptr<autobahn::transport_handler> transport_handler_ptr_t;

  virtual ~transport() = default;

  virtual void bind(std::string const& addr) = 0;
  virtual void listen() = 0;
  virtual void attach(transport_handler_ptr_t const& handler) = 0;
  virtual void detach() = 0;
  virtual void send_message(autobahn::message&& message) = 0;
};


// template <class T>
class transport_handler {
 public:
  typedef std::shared_ptr<autobahn::transport> transport_ptr_t;

  virtual ~transport_handler() = default;

  virtual void on_attach(transport_ptr_t const& transport) = 0;
  virtual void on_detach() = 0;
  virtual void on_message(autobahn::message&& message) = 0;
};

class zmq_transport : public transport,
                      public std::enable_shared_from_this<zmq_transport> {
 public:
  explicit zmq_transport(std::shared_ptr<zmq::context_t> const& context)
      : context_(context),
        socket_(*context, zmq::socket_type::pair),
        shutdown_socket_(*context, zmq::socket_type::pair) {
    // bind to our shutdown socket to receive the shutdown message send by the
    // shutdown function.
    shutdown_socket_.bind("inproc://shutdown_socket");
  }

  void bind(std::string const& addr) { socket_.bind(addr); }
  void connect(std::string const& addr) { socket_.connect(addr); }

  void send_message(autobahn::message&& message) {
    std::cerr << "Send Message: message_type="
              << static_cast<int>(message.message_type())
              << ", subject=" << message.subject() << std::endl;

    zmq::multipart_t msg;
    // msg.addstr(encode_message_type_to_string(message.message_type()));
    msg.addtyp<message_types>(message.message_type());
    msg.addstr(message.subject());
    msg.addstr(message.data());

    msg.send(socket_);
  }

  void attach(const transport_handler_ptr_t& handler) {
    if (handler_) {
      throw std::logic_error(
          "transport attach handler failed: a handler is already attached");
    }
    handler_ = handler;
    handler_->on_attach(
        std::static_pointer_cast<transport_t>(shared_from_this()));
  }

  void detach() {
    if (!handler_) {
      throw std::logic_error(
          "transport detach handler failed: no handler attached");
    }
    handler_->on_detach();
    handler_.reset();
  }

  void listen() {
    zmq::pollitem_t items[] = {{socket_, 0, ZMQ_POLLIN, 0},
                               {shutdown_socket_, 0, ZMQ_POLLIN, 0}};

    for (;;) {
      if (zmq::poll(items, 2, -1) > 0) {
        if (items[0].revents & ZMQ_POLLIN) {
          zmq::multipart_t msg;
          msg.recv(socket_);

          // auto message_type = decode_message_type_string(msg.popstr());
          auto message_type = msg.poptyp<message_types>();
          auto subject = msg.popstr();
          auto data = msg.popstr();

          std::cerr << "Recv Message: message_type="
                    << static_cast<int>(message_type) << ", subject=" << subject
                    << " data=[" << data << "]" << std::endl;

          if (handler_) {
            handler_->on_message(
                autobahn::make_message(message_type, subject, data));
          }
        }
        if (items[1].revents & ZMQ_POLLIN) {
          std::cerr << "shutdown signal received" << std::endl;
          break;
        }
      }
    }

    // Clean properly our sockets
    socket_.setsockopt(ZMQ_LINGER, 0);
    shutdown_socket_.setsockopt(ZMQ_LINGER, 0);

    socket_.close();
    shutdown_socket_.close();

    // Tell our shutdown function that we're leaving the listen function now.
    shutdown_.set_value(true);
  }

  void shutdown(size_t timeout) {
    // connect to our shutdown socket and send a message. This message will be
    // received by the listen function and breaks the loop. After cleanup of our
    // sockets we receive a future.
    zmq::socket_t socket(*context_, zmq::socket_type::pair);
    socket.connect("inproc://shutdown_socket");
    zmq::message_t shutdown_msg("0", 1);
    socket.send(shutdown_msg, zmq::send_flags::dontwait);

    auto shutdown_future = shutdown_.get_future();

    if (shutdown_future.wait_for(std::chrono::seconds(timeout)) ==
        std::future_status::timeout) {
      throw std::runtime_error(
          "transport shutdown failed: operation timed out");
    }
  }

 private:
  transport_handler_ptr_t handler_;
  std::shared_ptr<zmq::context_t> context_;
  zmq::socket_t socket_;
  zmq::socket_t shutdown_socket_;
  std::promise<bool> shutdown_;

  /*static std::string encode_message_type_to_string(message_types v) {
    switch (v) {
      case message_types::request:
        return "REQ";
      case message_types::reply:
        return "REP";
      case message_types::publish:
        return "PUB";
    }

    // This should never happen because compiler should prevent passing a wrong
    // message type. That's why we throw an exception here.
    throw std::runtime_error(
        "encode message type to string failed: invalid message type");
  }

  static message_types decode_message_type_string(std::string const& str) {
    if (str == "REQ") return message_types::request;
    if (str == "REP") return message_types::reply;
    if (str == "PUB") return message_types::publish;

    throw std::runtime_error(
        "decode message type string failed: invalid message type");
  }*/
};

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_TRANSPORT_HPP_
