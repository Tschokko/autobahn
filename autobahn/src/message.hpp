// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#ifndef AUTOBAHN_SRC_MESSAGE_HPP_
#define AUTOBAHN_SRC_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <tuple>
#include <utility>

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

class EventLearnAddress {
 public:
  EventLearnAddress(std::string const& common_name, std::string const& addr)
      : common_name_(common_name), addr_(addr) {}

  std::string common_name() const { return common_name_; }
  std::string addr() const { return addr_; }

  static std::string MarshalMsgPack(EventLearnAddress const& v) {
    std::stringstream ss;
    std::tuple<std::string, std::string> data(v.common_name_, v.addr_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static EventLearnAddress UnmarshalMsgPack(std::string const& str) {
    EventLearnAddress r;
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

  EventLearnAddress() {}
};

EventLearnAddress MakeEventLearnAddress(std::string const& common_name,
                                        std::string const& addr) {
  return EventLearnAddress(common_name, addr);
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
  static inline std::string const kLearnAddressSubject =
      "autobahn.learnaddress";

  // ------
  // hot
  // -------
  static inline std::string const kClientConnect = "autobahn.clientconnect";
};

enum class MessageType : int { kRequest = 1, kReply = 2, kPublish = 3 };

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
    // TODO(DGL) Add bad_cast exception handling
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

// -------
// Hot
// -------
enum class MessageTypes : int { kRequest = 1, kReply = 2, kPublish = 3 };

// template <template <class> class TT, class T>
class AbstractMessage {
 public:
  virtual MessageTypes message_type() = 0;
  virtual std::string subject() = 0;

  template <class T>
  T data() const;
  template <class T>
  void set_data(T const& data);
};

// --------
// Currrent
// --------
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

// template<class ENCODING>
class ZMQMessage {
 public:
  ZMQMessage(MessageTypes message_type, std::string const& subject,
             std::string const& data)
      : message_type_(message_type), subject_(subject), data_(data) {}
  ZMQMessage(const ZMQMessage& other) = delete;
  ZMQMessage(ZMQMessage&& other) {
    message_type_ = std::move(other.message_type_);
    subject_ = std::move(other.subject_);
    data_ = std::move(other.data_);
  }

  ZMQMessage& operator=(const ZMQMessage& other) = delete;
  ZMQMessage& operator=(ZMQMessage&& other) {
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
    return autobahn::message::MsgPackEncoding::DecodeString<T>(data_);
  }
  template <class T>
  void set_data(T const& v) {
    data_ = autobahn::message::MsgPackEncoding::EncodeToString<T>(v);
  }

 private:
  ZMQMessage() {}
  MessageTypes message_type_;
  std::string subject_;
  std::string data_;
};

ZMQMessage MakeZMQMessage(MessageTypes message_type, std::string const& subject,
                          std::string const& data) {
  return ZMQMessage(message_type, subject, data);
}

template <class T>
ZMQMessage MakeZMQMessage(MessageTypes const& message_type,
                          std::string const& subject, T const& v) {
  auto data = MsgPackEncoding::EncodeToString(v);
  return ZMQMessage(message_type, subject, data);
}


class ClientConnectRequest {
 public:
  explicit ClientConnectRequest(int request_id, std::string const& common_name)
      : request_id_(request_id), common_name_(common_name) {}

  ClientConnectRequest(const ClientConnectRequest& other) = delete;
  ClientConnectRequest(ClientConnectRequest&& other) {
    request_id_ = std::move(other.request_id_);
    common_name_ = std::move(other.common_name_);
  }

  ClientConnectRequest& operator=(const ClientConnectRequest& other) = delete;
  ClientConnectRequest& operator=(ClientConnectRequest&& other) {
    if (this == &other) {
      return *this;
    }

    request_id_ = std::move(other.request_id_);
    common_name_ = std::move(other.common_name_);

    return *this;
  }

  int request_id() const { return request_id_; }
  std::string common_name() const { return common_name_; }

 private:
  ClientConnectRequest() {}
  int request_id_;
  std::string common_name_;
};

ClientConnectRequest MakeClientConnectRequest(int request_id,
                                              std::string const& common_name) {
  return ClientConnectRequest(request_id, common_name);
}

class ClientConnectReqly {
 public:
  explicit ClientConnectReqly(int request_id, bool authorized,
                              std::string const& config)
      : request_id_(request_id), authorized_(authorized), config_(config) {}

  ClientConnectReqly(const ClientConnectReqly& other) = delete;
  ClientConnectReqly(ClientConnectReqly&& other) {
    request_id_ = std::move(other.request_id_);
    authorized_ = std::move(other.authorized_);
    config_ = std::move(other.config_);
  }

  ClientConnectReqly& operator=(const ClientConnectReqly& other) = delete;
  ClientConnectReqly& operator=(ClientConnectReqly&& other) {
    if (this == &other) {
      return *this;
    }

    request_id_ = std::move(other.request_id_);
    authorized_ = std::move(other.authorized_);
    config_ = std::move(other.config_);

    return *this;
  }

  int request_id() const { return request_id_; }
  bool authorized() const { return authorized_; }
  std::string config() const { return config_; }

 private:
  ClientConnectReqly() {}
  int request_id_;
  bool authorized_;
  std::string config_;
};

ClientConnectReqly MakeClientConnectReqly(int request_id, bool authorized,
                                          std::string const& config) {
  return ClientConnectReqly(request_id, authorized, config);
}

}  // namespace autobahn


#endif  // AUTOBAHN_SRC_MESSAGE_HPP_
