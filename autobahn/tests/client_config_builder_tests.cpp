#include "autobahn/src/client_config_builder.hpp"
#include "boost/asio/ip/address_v4.hpp"
#include "boost/asio/ip/network_v4.hpp"
#include "gtest/gtest.h"

TEST(openvpn_ClientConfigBuilder, BuildConfigWithIPv4Route_HasPushIPv4Route) {
  // Arrange
  auto route = boost::asio::ip::make_network_v4("192.168.2.0/24");

  std::stringstream expected;
  expected << "push \"route " << route.address().to_string() << " "
           << route.netmask().to_string() << "\"\n";

  autobahn::openvpn::ClientConfigBuilder cc;
  cc.AddIPv4Route(route);

  // Act
  auto actual = cc.BuildConfig();

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(openvpn_ClientConfigBuilder, BuildConfigWithIPv6Route_HasPushIPv6Route) {
  // Arrange
  auto route = boost::asio::ip::make_network_v6("2001:db8:cafe::/64");

  std::stringstream expected;
  expected << "push \"route-ipv6 " << route.to_string() << "\"\n";

  autobahn::openvpn::ClientConfigBuilder cc;
  cc.AddIPv6Route(route);

  // Act
  auto actual = cc.BuildConfig();

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(openvpn_ClientConfigBuilder, BuildConfigWithIPv4Network_HasIPv4IRoute) {
  // Arrange
  auto network = boost::asio::ip::make_network_v4("192.168.2.0/24");

  std::stringstream expected;
  expected << "iroute " << network.address().to_string() << " "
           << network.netmask().to_string() << "\n";

  autobahn::openvpn::ClientConfigBuilder cc;
  cc.AddIPv4Network(network);

  // Act
  auto actual = cc.BuildConfig();

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(openvpn_ClientConfigBuilder, BuildConfigWithIPv6Network_HasIPv6IRoute) {
  // Arrange
  auto network = boost::asio::ip::make_network_v6("2001:db8:cafe::/64");

  std::stringstream expected;
  expected << "iroute-ipv6 " << network.to_string() << "\n";

  autobahn::openvpn::ClientConfigBuilder cc;
  cc.AddIPv6Network(network);

  // Act
  auto actual = cc.BuildConfig();

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(openvpn_ClientConfigBuilder,
     BuildConfigWithIPv4InterfaceConfig_HasIPv4InterfaceConfig) {
  // Arrange
  auto ifcfg = boost::asio::ip::make_network_v4("10.18.0.3/24");

  std::stringstream expected;
  expected << "ifconfig-push " << ifcfg.address().to_string() << " "
           << ifcfg.netmask().to_string() << "\n";

  autobahn::openvpn::ClientConfigBuilder cc;
  cc.SetIPv4InterfaceConfig(ifcfg);

  // Act
  auto actual = cc.BuildConfig();

  // Assert
  EXPECT_EQ(actual, expected.str());
}

TEST(openvpn_ClientConfigBuilder,
     BuildConfigWithIPv6InterfaceConfig_HasIPv6InterfaceConfig) {
  // Arrange
  auto ifcfg = boost::asio::ip::make_network_v6("2001:db8::3/112");

  std::stringstream expected;
  expected << "ifconfig-ipv6-push " << ifcfg.to_string() << "\n";

  autobahn::openvpn::ClientConfigBuilder cc;
  cc.SetIPv6InterfaceConfig(ifcfg);

  // Act
  auto actual = cc.BuildConfig();

  // Assert
  EXPECT_EQ(actual, expected.str());
}
