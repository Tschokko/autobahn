// Copyright (c) 2019 by tschokko.de.
// Author: Tschokko

#include "autobahn/src/client_config.hpp"
#include "autobahn/src/client_config_builder.hpp"
#include "boost/asio/ip/address_v4.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "gtest/gtest.h"

TEST(client_config_builder, build_config_with_ipv4_route__has_push_ipv4_route) {
  // Arrange
  auto route = boost::asio::ip::make_network_v4("192.168.2.0/24");

  std::stringstream expected;
  expected << "push \"route " << route.address().to_string() << " "
           << route.netmask().to_string() << "\"\n";

  autobahn::client_config config;
  config.add_ipv4_route(route);

  // Act
  auto actual = autobahn::client_config_builder::build(config);

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(client_config_builder, build_config_with_ipv6_route__has_push_ipv6_route) {
  // Arrange
  auto route = boost::asio::ip::make_network_v6("2001:db8:cafe::/64");

  std::stringstream expected;
  expected << "push \"route-ipv6 " << route.to_string() << "\"\n";

  autobahn::client_config config;
  config.add_ipv6_route(route);

  // Act
  auto actual = autobahn::client_config_builder::build(config);

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(client_config_builder, build_config_with_ipv4_network__has_ipv4_iroute) {
  // Arrange
  auto network = boost::asio::ip::make_network_v4("192.168.2.0/24");

  std::stringstream expected;
  expected << "iroute " << network.address().to_string() << " "
           << network.netmask().to_string() << "\n";

  autobahn::client_config config;
  config.add_ipv4_network(network);

  // Act
  auto actual = autobahn::client_config_builder::build(config);

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(client_config_builder, build_config_with_ipv6_network__has_ipv6_iroute) {
  // Arrange
  auto network = boost::asio::ip::make_network_v6("2001:db8:cafe::/64");

  std::stringstream expected;
  expected << "iroute-ipv6 " << network.to_string() << "\n";

  autobahn::client_config config;
  config.add_ipv6_network(network);

  // Act
  auto actual = autobahn::client_config_builder::build(config);

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(client_config_builder,
     build_config_with_ipv4_interface_config__has_ipv4_interface_config) {
  // Arrange
  auto ifcfg = boost::asio::ip::make_network_v4("10.18.0.3/24");

  std::stringstream expected;
  expected << "ifconfig-push " << ifcfg.address().to_string() << " "
           << ifcfg.netmask().to_string() << "\n";

  autobahn::client_config config;
  config.set_ipv4_interface_config(ifcfg);

  // Act
  auto actual = autobahn::client_config_builder::build(config);

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(client_config_builder,
     build_config_with_ipv6_interface_config__has_ipv6_interface_config) {
  // Arrange
  auto ifcfg = boost::asio::ip::make_network_v6("2001:db8::3/112");

  std::stringstream expected;
  expected << "ifconfig-ipv6-push " << ifcfg.to_string() << "\n";

  autobahn::client_config config;
  config.set_ipv6_interface_config(ifcfg);

  // Act
  auto actual = autobahn::client_config_builder::build(config);

  // Assert
  EXPECT_EQ(actual, expected.str());
}
