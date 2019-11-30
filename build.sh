#!/bin/sh

#
# Build an OpenVPN plugin module on *nix.  The argument should
# be the base name of the C source file (without the .c).
#

# This directory is where we will look for openvpn-plugin.h
# CPPFLAGS="${CPPFLAGS:--I/home/user/rootfs_staging/rootfs_staging/include}"

CC="${CC:-g++}"
CFLAGS="${CFLAGS:--O2 -Wall -g -I/usr/include/openvpn -I./include}"
LDFLAGS=-lssl

$CC $CPPFLAGS $CFLAGS -fPIC -c -o build/autobahn-plugin.o src/autobahn-plugin.cpp && \
$CC $CFLAGS -fPIC -shared $LDFLAGS -Wl,-soname,autobahn-plugin.so -o bin/autobahn-plugin.so build/autobahn-plugin.o -lc
