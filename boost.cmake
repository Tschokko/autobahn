include(ExternalProject)

set(Boost_URL "https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz")
# set(Boost_SHA1 "9f1dd4fa364a3e3156a77dc17aa562ef06404ff6")
set(Boost_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/third_party/boost)
set(Boost_INCLUDE_DIR ${Boost_INSTALL}/include)
set(Boost_LIB_DIR ${Boost_INSTALL}/lib)

ExternalProject_Add(boost
        PREFIX boost
        URL ${Boost_URL}
        # URL_HASH SHA1=${boost_SHA1}
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND
        ./bootstrap.sh
        --with-libraries=system
        --with-libraries=filesystem
        #--with-libraries=process
        --prefix=<INSTALL_DIR>
        BUILD_COMMAND
        ./b2 install link=static variant=release threading=multi runtime-link=static
        INSTALL_COMMAND ""
        INSTALL_DIR ${Boost_INSTALL} )

set(Boost_LIBRARIES
        ${Boost_LIB_DIR}/libboost_filesystem.a
        ${Boost_LIB_DIR}/libboost_system.a )
message(STATUS "Boost static libs: " ${Boost_LIBRARIES})

#set(MSGPACK_URL "https://github.com/msgpack/msgpack-c/releases/download/cpp-3.2.0/msgpack-3.2.0.tar.gz")
#set(MSGPACK_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/third_party/msgpack)
#set(MSGPACK_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/msgpack/include)
#ExternalProject_Add(msgpack
#  PREFIX msgpack
#  URL ${MSGPACK_URL}
#  SOURCE_DIR "${MSGPACK_INSTALL}"
#  CMAKE_CACHE_ARGS
#    -DMSGPACK_CXX17:string=ON
#  #        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/msgpack
#  INSTALL_COMMAND ""
#)


#set(ZeroMQ_URL "https://github.com/zeromq/libzmq/releases/download/v4.3.2/zeromq-4.3.2.tar.gz")
#set(ZeroMQ_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/third_party/zeromq)
#set(ZeroMQ_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/zeromq/include)
#ExternalProject_Add(zeromq
#  PREFIX zeromq
#  URL ${ZeroMQ_URL}
#  SOURCE_DIR "${ZeroMQ_INSTALL}"
  #CMAKE_CACHE_ARGS
  #  -DMSGPACK_CXX17:string=ON
  #        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/msgpack
#  INSTALL_COMMAND ""
#)


#set(CppZMQ_URL "https://github.com/zeromq/cppzmq/archive/v4.5.0.tar.gz")
#set(CppZMQ_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/third_party/cppzmq)
#set(CppZMQ_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/cppzmq)
#ExternalProject_Add(cppzmq
#  PREFIX cppzmq
#  URL ${CppZMQ_URL}
#  SOURCE_DIR "${CppZMQ_INSTALL}"
#  CMAKE_CACHE_ARGS
#    -DCPPZMQ_BUILD_TESTS:string=OFF
#  #        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/msgpack
#  INSTALL_COMMAND ""
#)

#set(NATS_URL "https://github.com/nats-io/nats.c/archive/v2.1.0.tar.gz")
#set(NATS_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/third_party/nats)
#set(NATS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/nats/src)
#ExternalProject_Add(nats
#  PREFIX nats
#  URL ${NATS_URL}
#  SOURCE_DIR "${NATS_INSTALL}"
#  CMAKE_CACHE_ARGS
#    -DNATS_BUILD_STREAMING:string=OFF
#  #        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/msgpack
#  INSTALL_COMMAND ""
#)

