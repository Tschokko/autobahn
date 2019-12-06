// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_MESSAGE_HPP_
#define AUTOBAHN_SRC_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <tuple>
#include <utility>

#include "msgpack.hpp"

namespace autobahn {

enum class MessageTypes : int { kRequest = 1, kReply = 2, kPublish = 3 };

struct Subjects {
  static inline std::string const kClientConnect = "clientconnect";
  static inline std::string const kLearnAddress = "learnaddress";
};

class MsgPackEncoding {
 public:
  typedef std::string Type;

  template <class T>
  static Type EncodeToString(T const& v) {
    return T::MarshalMsgPack(v);
  }

  template <class T>
  static T DecodeString(Type const& str) {
    return T::UnmarshalMsgPack(str);
  }
};

class Message {
 public:
  Message(MessageTypes message_type, std::string const& subject,
          std::string const& data)
      : message_type_(message_type), subject_(subject), data_(data) {}
  Message(const Message& other) = delete;
  Message(Message&& other) {
    message_type_ = std::move(other.message_type_);
    subject_ = std::move(other.subject_);
    data_ = std::move(other.data_);
  }

  Message& operator=(const Message& other) = delete;
  Message& operator=(Message&& other) {
    if (this == &other) {
      return *this;
    }

    message_type_ = std::move(other.message_type_);
    subject_ = std::move(other.subject_);
    data_ = std::move(other.data_);

    return *this;
  }

  // accessors & mutators
  MessageTypes message_type() const { return message_type_; }
  void set_message_type(MessageTypes message_type) {
    message_type_ = message_type;
  }
  std::string subject() const { return subject_; }
  void set_subject(std::string const& subject) { subject_ = subject; }
  std::string data() const { return data_; }
  void set_data(std::string const& data) { data_ = data; }

  template <class T>
  T data() const {
    return autobahn::MsgPackEncoding::DecodeString<T>(data_);
  }
  template <class T>
  void set_data(T const& v) {
    data_ = autobahn::MsgPackEncoding::EncodeToString<T>(v);
  }

 private:
  Message() {}
  MessageTypes message_type_;
  std::string subject_;
  std::string data_;
};

Message MakeMessage(MessageTypes message_type, std::string const& subject,
                    std::string const& data) {
  return Message(message_type, subject, data);
}

template <class T>
Message MakeMessage(MessageTypes const& message_type,
                    std::string const& subject, T const& v) {
  auto data = MsgPackEncoding::EncodeToString(v);
  return Message(message_type, subject, data);
}


class ClientConnectRequest {
 public:
  explicit ClientConnectRequest(int request_id, std::string const& common_name)
      : request_id_(request_id), common_name_(common_name) {}

  // MsgPackEncoding Support
  static std::string MarshalMsgPack(ClientConnectRequest const& v) {
    std::stringstream ss;
    std::tuple<int, std::string> data(v.request_id_, v.common_name_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static ClientConnectRequest UnmarshalMsgPack(std::string const& str) {
    ClientConnectRequest r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, std::string>>();
    r.request_id_ = std::get<0>(data);
    r.common_name_ = std::get<1>(data);
    return r;
  }

  // accessors & mutators
  int request_id() const { return request_id_; }
  std::string common_name() const { return common_name_; }

 private:
  int request_id_;
  std::string common_name_;

  // Hide default constructor
  ClientConnectRequest() {}
};

ClientConnectRequest MakeClientConnectRequest(int request_id,
                                              std::string const& common_name) {
  return ClientConnectRequest(request_id, common_name);
}

class ClientConnectReply {
 public:
  explicit ClientConnectReply(int request_id, bool authorized,
                              std::string const& config)
      : request_id_(request_id), authorized_(authorized), config_(config) {}

  // MsgPackEncoding Support
  static std::string MarshalMsgPack(ClientConnectReply const& v) {
    std::stringstream ss;
    std::tuple<int, bool, std::string> data(v.request_id_, v.authorized_,
                                            v.config_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static ClientConnectReply UnmarshalMsgPack(std::string const& str) {
    ClientConnectReply r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, bool, std::string>>();
    r.request_id_ = std::get<0>(data);
    r.authorized_ = std::get<1>(data);
    r.config_ = std::get<2>(data);
    return r;
  }

  // Accessors & Mutators
  int request_id() const { return request_id_; }
  bool authorized() const { return authorized_; }
  std::string config() const { return config_; }

 private:
  int request_id_;
  bool authorized_;
  std::string config_;

  // Hide default constructor
  ClientConnectReply() {}
};

ClientConnectReply MakeClientConnectReply(int request_id, bool authorized,
                                          std::string const& config) {
  return ClientConnectReply(request_id, authorized, config);
}

/* class LearnAddressEvent {
 public:
  LearnAddressEvent(std::string const& common_name, std::string const& addr)
      : common_name_(common_name), addr_(addr) {}

  std::string common_name() const { return common_name_; }
  std::string addr() const { return addr_; }

  static std::string MarshalMsgPack(LearnAddressEvent const& v) {
    std::stringstream ss;
    std::tuple<std::string, std::string> data(v.common_name_, v.addr_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static LearnAddressEvent UnmarshalMsgPack(std::string const& str) {
    LearnAddressEvent r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<std::string, std::string>>();
    r.common_name_ = std::get<0>(data);
    r.addr_ = std::get<1>(data);
    return r;
  }

 private:
  std::string common_name_;
  std::string addr_;

  // Hide default constructor
  LearnAddressEvent() {}
};

LearnAddressEvent MakeLearnAddressEvent(std::string const& common_name,
                                        std::string const& addr) {
  return LearnAddressEvent(common_name, addr);
}*/

enum class LearnAddressOperations : int { kAdd = 1, kUpdate = 2, kDelete = 3 };

class LearnAddressRequest {
 public:
  explicit LearnAddressRequest(int request_id, LearnAddressOperations operation, std::string const& address, std::string const& common_name)
      : request_id_(request_id), operation_(operation), address_(address), common_name_(common_name) {}

  // MsgPackEncoding Support
  static std::string MarshalMsgPack(LearnAddressRequest const& v) {
    std::stringstream ss;
    std::tuple<int, int, std::string, std::string> data(v.request_id_, static_cast<int>(v.operation_), v.address_, v.common_name_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static LearnAddressRequest UnmarshalMsgPack(std::string const& str) {
    LearnAddressRequest r;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, int, std::string, std::string>>();
    r.request_id_ = std::get<0>(data);
    r.operation_ = static_cast<LearnAddressOperations>(std::get<1>(data));
    r.address_ = std::get<2>(data);
    r.common_name_ = std::get<3>(data);
    return r;
  }

  // accessors & mutators
  int request_id() const { return request_id_; }
  std::string common_name() const { return common_name_; }

 private:
  int request_id_;
  LearnAddressOperations operation_;
  std::string address_;
  std::string common_name_;

  // Hide default constructor
  LearnAddressRequest() {}
};

LearnAddressRequest MakeLearnAddressRequest(int request_id, LearnAddressOperations operation,
                                              std::string const& address, std::string const& common_name) {
  return LearnAddressRequest(request_id, operation, address, common_name);
}

class LearnAddressReply {
 public:
  explicit LearnAddressReply(int request_id, bool learned)
      : request_id_(request_id), learned_(learned) {}

  // MsgPackEncoding Support
  static std::string MarshalMsgPack(LearnAddressReply const& v) {
    std::stringstream ss;
    std::tuple<int, bool> data(v.request_id_, v.learned_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static LearnAddressReply UnmarshalMsgPack(std::string const& str) {
    LearnAddressReply v;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, bool>>();
    v.request_id_ = std::get<0>(data);
    v.learned_ = std::get<1>(data);
    return v;
  }

  // Accessors & Mutators
  int request_id() const { return request_id_; }
  bool learned() const { return learned_; }

 private:
  int request_id_;
  bool learned_;

  // Hide default constructor
  LearnAddressReply() {}
};

LearnAddressReply MakeLearnAddressReply(int request_id, bool learned) {
  return LearnAddressReply(request_id, learned);
}

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_MESSAGE_HPP_
