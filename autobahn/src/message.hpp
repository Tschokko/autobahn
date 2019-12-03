// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_MESSAGE_HPP_
#define AUTOBAHN_SRC_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <tuple>

#include "msgpack.hpp"

namespace autobahn::message {

class RequestClientConnect {
 public:
  explicit RequestClientConnect(std::string const& common_name)
      : common_name_(common_name) {}

  std::string common_name() const { return common_name_; }

  static std::string MarshalMsgPack(RequestClientConnect const& v) {
    std::stringstream ss;
    msgpack::pack(ss, v.common_name_);
    return ss.str();
  }

  static RequestClientConnect UnmarshalMsgPack(std::string const& str) {
    RequestClientConnect r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    r.common_name_ = obj.as<std::string>();
    return r;
  }

 private:
  RequestClientConnect() {}

  std::string common_name_;
};

RequestClientConnect MakeRequestClientConnect(std::string const& common_name) {
  return RequestClientConnect(common_name);
}

class ReplyClientConnect {
 public:
  ReplyClientConnect(bool authorized, std::string const& config)
      : authorized_(authorized), config_(config) {}

  bool authorized() const { return authorized_; }
  std::string config() const { return config_; }

  static std::string MarshalMsgPack(ReplyClientConnect const& v) {
    std::stringstream ss;
    std::tuple<bool, std::string> data(v.authorized_, v.config_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static ReplyClientConnect UnmarshalMsgPack(std::string const& str) {
    ReplyClientConnect r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<bool, std::string>>();
    r.authorized_ = std::get<0>(data);
    r.config_ = std::get<1>(data);
    return r;
  }

 private:
  ReplyClientConnect() {}

  bool authorized_;
  std::string config_;
};

ReplyClientConnect MakeReplyClientConnect(bool authorized,
                                          std::string const& config) {
  return ReplyClientConnect(authorized, config);
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

  std::string subject() const { return subject_; }
  T data() const { return data_; }

 private:
  MessageType message_type_;
  std::string subject_;
  std::string reply_;
  T data_;
};

template <class T>
Message<T> MakeMessage(MessageType const& message_type,
                       std::string const& subject, T const& data) {
  return Message<T>(message_type, subject, data);
}

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_MESSAGE_HPP_
