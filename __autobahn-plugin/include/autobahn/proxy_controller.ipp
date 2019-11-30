#include <iostream>
#include <map>

namespace autobahn {

inline bool ProxyController::Connect(const std::string& addr) { return true; }

inline bool ProxyController::RequestClientConnect(
    const std::string& common_name, ClientConfig& client_config) {
  std::cout << "ProxyController::RequstClientConnect" << std::endl;

  /*"push \"route 10.11.12.0 255.255.255.0\"\npush \"route 10.11.12.13 "
        "255.255.255.255\"");*/
  client_config.AddIPv4Route("10.11.12.0 255.255.255.0");
  client_config.AddIPv4Route("10.11.12.13 255.255.255.255");

  return true;
}

inline void ProxyController::PublishAddAddress(const std::string& addr,
                                               const std::string& common_name) {
  std::cout << "ProxyController::PublishAddAddress addr=" << addr
            << " for CN=" << common_name << std::endl;
}

inline void ProxyController::PublishDeleteAddress(const std::string& addr) {
  std::cout << "ProxyController::PublishDeleteAddress addr=" << addr
            << std::endl;
}

inline void ProxyController::PublishClientDisconnect(
    const std::string& common_name) {
  std::cout << "ProxyController::PublishClientDisconnect common_name="
            << common_name << std::endl;
}

}  // namespace autobahn
