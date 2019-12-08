// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include "boost/asio/ip/address_v4.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "gtest/gtest.h"

#include "autobahn/src/openvpn/config.hpp"
#include "autobahn/src/openvpn/config_builder.hpp"

TEST(openvpn__config_builder, build_args_with_port__has_port_arg) {
  // Arrange
  std::string expected = "--port 1194";

  autobahn::openvpn::config config;
  config.set_port(1194);

  // Act
  auto actual = autobahn::openvpn::config_builder::build_args(config);

  // Assert
  ASSERT_EQ(actual.size(), 1);
  EXPECT_EQ(actual[0], expected);
}

TEST(openvpn__config_builder,
     build_args_with_protocol_udp__has_protocol_udp_arg) {
  // Arrange
  std::string expected = "--proto udp";

  autobahn::openvpn::config config;
  config.set_protocol(autobahn::openvpn::protocols::udp);

  // Act
  auto actual = autobahn::openvpn::config_builder::build_args(config);

  // Assert
  ASSERT_EQ(actual.size(), 1);
  EXPECT_EQ(actual[0], expected);
}
