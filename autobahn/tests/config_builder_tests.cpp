#include "autobahn/src/config_builder.hpp"
#include "boost/asio/ip/address_v4.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "gtest/gtest.h"

TEST(openvpn_ConfigBuilder, BuildArgsWithPort_HasPortArg) {
  // Arrange
  std::string expected = "--port 1194";

  autobahn::openvpn::ConfigBuilder config;
  config.SetPort(1194);

  // Act
  auto actual = config.BuildArgs();

  // Assert
  ASSERT_EQ(actual.size(), 1);
  EXPECT_EQ(actual[0], expected);
}

TEST(openvpn_ConfigBuilder, BuildArgsWithProtocol_HasProtocolArg) {
  // Arrange
  std::string expected = "--proto udp";

  autobahn::openvpn::ConfigBuilder config;
  config.SetProtocol(autobahn::openvpn::kProtocolUDP);

  // Act
  auto actual = config.BuildArgs();

  // Assert
  ASSERT_EQ(actual.size(), 1);
  EXPECT_EQ(actual[0], expected);
}
