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

enum class message_types : int { request = 1, reply = 2, publish = 3 };

struct message_subjects {
  static inline std::string const clientconnect = "clientconnect";
  static inline std::string const learnaddress = "learnaddress";
};

class msgpack_encoding {
 public:
  template <class T>
  static std::string encode_to_string(T const& v) {
    return T::marshal_msgpack(v);
  }

  template <class T>
  static T decode_string(std::string const& str) {
    return T::unmarshal_msgpack(str);
  }
};

class message {
 public:
  message(message_types message_type, std::string const& subject,
          std::string const& data)
      : message_type_(message_type), subject_(subject), data_(data) {}
  message(const message& other) = delete;
  message(message&& other) {
    message_type_ = std::move(other.message_type_);
    subject_ = std::move(other.subject_);
    data_ = std::move(other.data_);
  }

  message& operator=(const message& other) = delete;
  message& operator=(message&& other) {
    if (this == &other) {
      return *this;
    }

    message_type_ = std::move(other.message_type_);
    subject_ = std::move(other.subject_);
    data_ = std::move(other.data_);

    return *this;
  }

  // accessors & mutators
  message_types message_type() const { return message_type_; }
  void set_message_type(message_types message_type) {
    message_type_ = message_type;
  }
  std::string subject() const { return subject_; }
  void set_subject(std::string const& subject) { subject_ = subject; }
  std::string data() const { return data_; }
  void set_data(std::string const& data) { data_ = data; }

  template <class T>
  T data() const {
    return autobahn::msgpack_encoding::decode_string<T>(data_);
  }
  template <class T>
  void set_data(T const& v) {
    data_ = autobahn::msgpack_encoding::encode_to_string<T>(v);
  }

 private:
  message() {}
  message_types message_type_;
  std::string subject_;
  std::string data_;
};

message make_message(message_types message_type, std::string const& subject,
                     std::string const& data) {
  return message(message_type, subject, data);
}

template <class T>
message make_message(message_types const& message_type,
                     std::string const& subject, T const& v) {
  auto data = msgpack_encoding::encode_to_string(v);
  return message(message_type, subject, data);
}

class client_connect_request {
 public:
  explicit client_connect_request(int request_id,
                                  std::string const& common_name)
      : request_id_(request_id), common_name_(common_name) {}

  // msgpack_encoding Support
  static std::string marshal_msgpack(client_connect_request const& v) {
    std::stringstream ss;
    std::tuple<int, std::string> data(v.request_id_, v.common_name_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static client_connect_request unmarshal_msgpack(std::string const& str) {
    client_connect_request v;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, std::string>>();
    v.request_id_ = std::get<0>(data);
    v.common_name_ = std::get<1>(data);
    return v;
  }

  // accessors & mutators
  int request_id() const { return request_id_; }
  std::string common_name() const { return common_name_; }

 private:
  int request_id_;
  std::string common_name_;

  // Hide default constructor
  client_connect_request() {}
};

client_connect_request make_client_connect_request(
    int request_id, std::string const& common_name) {
  return client_connect_request(request_id, common_name);
}

class client_connect_reply {
 public:
  explicit client_connect_reply(int request_id, bool valid,
                                std::string const& config)
      : request_id_(request_id), valid_(valid), config_(config) {}

  // msgpack_encoding Support
  static std::string marshal_msgpack(client_connect_reply const& v) {
    std::stringstream ss;
    std::tuple<int, bool, std::string> data(v.request_id_, v.valid_, v.config_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static client_connect_reply unmarshal_msgpack(std::string const& str) {
    client_connect_reply v;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, bool, std::string>>();
    v.request_id_ = std::get<0>(data);
    v.valid_ = std::get<1>(data);
    v.config_ = std::get<2>(data);
    return v;
  }

  // Accessors & Mutators
  int request_id() const { return request_id_; }
  void set_request_id(int request_id) { request_id_ = request_id; }
  bool valid() const { return valid_; }
  void set_valid(bool valid) { valid_ = valid; }
  std::string config() const { return config_; }
  void set_config(std::string const& config) { config_ = config; }

 private:
  int request_id_;
  bool valid_;
  std::string config_;

  // Hide default constructor
  client_connect_reply() {}
};

client_connect_reply make_client_connect_reply(int request_id, bool valid,
                                               std::string const& config) {
  return client_connect_reply(request_id, valid, config);
}

enum class learn_address_operations : int { add = 1, update = 2, remove = 3 };

learn_address_operations learn_address_operation_from_string(
    std::string const& str) {
  if (str == "add") {
    return learn_address_operations::add;
  }
  if (str == "update") {
    return learn_address_operations::update;
  }
  if (str == "delete") {
    return learn_address_operations::remove;
  }

  // This should never happen inside a OpenVPN plugin. It's fine to crash the
  // plugin if we receive an invalid string.
  throw std::logic_error("invalid operations string");
}

std::string learn_address_operation_to_string(
    learn_address_operations operation) {
  switch (operation) {
    case learn_address_operations::add:
      return "add";
    case learn_address_operations::update:
      return "update";
    case learn_address_operations::remove:
      return "delete";
  }
}

class learn_address_request {
 public:
  explicit learn_address_request(int request_id,
                                 learn_address_operations operation,
                                 std::string const& address,
                                 std::string const& common_name)
      : request_id_(request_id),
        operation_(operation),
        address_(address),
        common_name_(common_name) {}

  // msgpack_encoding Support
  static std::string marshal_msgpack(learn_address_request const& v) {
    std::stringstream ss;
    std::tuple<int, int, std::string, std::string> data(
        v.request_id_, static_cast<int>(v.operation_), v.address_,
        v.common_name_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static learn_address_request unmarshal_msgpack(std::string const& str) {
    learn_address_request v;
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    auto data = obj.as<std::tuple<int, int, std::string, std::string>>();
    v.request_id_ = std::get<0>(data);
    v.operation_ = static_cast<learn_address_operations>(std::get<1>(data));
    v.address_ = std::get<2>(data);
    v.common_name_ = std::get<3>(data);
    return v;
  }

  // accessors & mutators
  int request_id() const { return request_id_; }
  learn_address_operations operation() const { return operation_; }
  std::string address() const { return address_; }
  std::string common_name() const { return common_name_; }

 private:
  int request_id_;
  learn_address_operations operation_;
  std::string address_;
  std::string common_name_;

  // Hide default constructor
  learn_address_request() {}
};

learn_address_request make_learn_address_request(
    int request_id, learn_address_operations operation,
    std::string const& address, std::string const& common_name) {
  return learn_address_request(request_id, operation, address, common_name);
}

class learn_address_reply {
 public:
  explicit learn_address_reply(int request_id, bool learned)
      : request_id_(request_id), learned_(learned) {}

  // msgpack_encoding Support
  static std::string marshal_msgpack(learn_address_reply const& v) {
    std::stringstream ss;
    std::tuple<int, bool> data(v.request_id_, v.learned_);
    msgpack::pack(ss, data);
    return ss.str();
  }

  static learn_address_reply unmarshal_msgpack(std::string const& str) {
    learn_address_reply v;
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
  learn_address_reply() {}
};

learn_address_reply make_learn_address_reply(int request_id, bool learned) {
  return learn_address_reply(request_id, learned);
}

}  // namespace autobahn

#endif  // AUTOBAHN_SRC_MESSAGE_HPP_
