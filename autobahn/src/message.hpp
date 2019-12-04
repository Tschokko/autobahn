// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_MESSAGE_HPP_
#define AUTOBAHN_SRC_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <tuple>

#include "msgpack.hpp"

namespace autobahn::message {

class WithRequestID {
 public:
  explicit WithRequestID(int request_id) : request_id_(request_id) {}
  int request_id() const { return request_id_; }

 protected:
  int request_id_;
};

class RequestClientConnect : public WithRequestID {
 public:
  explicit RequestClientConnect(uint request_id, std::string const& common_name)
      : WithRequestID(request_id), common_name_(common_name) {}

  std::string common_name() const { return common_name_; }

  static std::string MarshalMsgPack(RequestClientConnect const& v) {
    std::stringstream ss;
    std::tuple<int, std::string> data(v.request_id_, v.common_name_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static RequestClientConnect UnmarshalMsgPack(std::string const& str) {
    RequestClientConnect r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, std::string>>();
    r.request_id_ = std::get<0>(data);
    r.common_name_ = std::get<1>(data);
    return r;
  }

 private:
  std::string common_name_;

  RequestClientConnect() : WithRequestID(0) {}
};

RequestClientConnect MakeRequestClientConnect(uint request_id,
                                              std::string const& common_name) {
  return RequestClientConnect(request_id, common_name);
}

class ReplyClientConnect : public WithRequestID {
 public:
  ReplyClientConnect(uint request_id, bool authorized,
                     std::string const& config)
      : WithRequestID(request_id), authorized_(authorized), config_(config) {}

  bool authorized() const { return authorized_; }
  std::string config() const { return config_; }

  static std::string MarshalMsgPack(ReplyClientConnect const& v) {
    std::stringstream ss;
    std::tuple<int, bool, std::string> data(v.request_id_, v.authorized_,
                                            v.config_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static ReplyClientConnect UnmarshalMsgPack(std::string const& str) {
    ReplyClientConnect r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, bool, std::string>>();
    r.request_id_ = std::get<0>(data);
    r.authorized_ = std::get<1>(data);
    r.config_ = std::get<2>(data);
    return r;
  }

 private:
  ReplyClientConnect() : WithRequestID(0) {}

  bool authorized_;
  std::string config_;
};

ReplyClientConnect MakeReplyClientConnect(uint request_id, bool authorized,
                                          std::string const& config) {
  return ReplyClientConnect(request_id, authorized, config);
}

class MsgPackEncoding {
 public:
  template <class T>
  static std::string EncodeToString(T const& v) {
    return T::MarshalMsgPack(v);
  }

  template <class T>
  static T DecodeString(std::string const& str) {
    return T::UnmarshalMsgPack(str);
  }
};

}  // namespace autobahn::message

namespace autobahn {

struct Subjects {
  static inline std::string const kClientConnectSubject =
      "autobahn.clientconnect";
};

enum class MessageType : int { kRequest = 1, kReply = 2 };

template <class T>
class Message {
 public:
  Message(MessageType const& message_type, std::string const& subject,
          T const& data)
      : message_type_(message_type), subject_(subject), data_(data) {}

  Message(const Message& other) = delete;
  Message(Message&& other) {
    message_type_ = other.message_type_;
    subject_ = other.subject_;
    reply_ = other.reply_;
    data_ = std::move(other.data_);
  }

  Message& operator=(const Message& other) = delete;
  Message& operator=(Message&& other) {
    if (this == &other) {
      return *this;
    }

    message_type_ = other.message_type_;
    subject_ = other.subject_;
    reply_ = other.reply_;
    data_ = std::move(other.data_);

    return *this;
  }

  MessageType message_type() const { return message_type_; }
  std::string subject() const { return subject_; }
  T data() const { return data_; }

  template <class D>
  D data() const {
    return autobahn::message::MsgPackEncoding::DecodeString<D>(data_);
  }

 private:
  MessageType message_type_;
  std::string subject_;
  std::string reply_;
  T data_;
};

/*template <class T>
Message<T> MakeMessage(MessageType const& message_type,
                       std::string const& subject, T const& data) {
  return Message<T>(message_type, subject, data);
}*/

template <class T, class D>
Message<T> MakeMessage(MessageType const& message_type,
                       std::string const& subject, D const& data) {
  auto d = autobahn::message::MsgPackEncoding::EncodeToString(data);
  return Message<T>(message_type, subject, d);
}

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_MESSAGE_HPP_
