load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")

cmake_external(
    name = "libzmq",
    # Values to be passed as -Dkey=value on the CMake command line;
    # here are serving to provide some CMake script configuration options
    # cache_entries = {
    #     "NOFORTRAN": "on",
    #     "BUILD_WITHOUT_LAPACK": "no",
    # },
    lib_source = "@libzmq//:all",
    static_libraries = ["libzmq.a"],
)

cmake_external(
    name = "cppzmq",
    cache_entries = {
        "CPPZMQ_BUILD_TESTS": "OFF",
    },
    env_vars = {
        "ZeroMQ_DIR": "$EXT_BUILD_DEPS/libzmq",
    },
    headers_only = True,
    lib_source = "@cppzmq//:all",
    deps = [
        ":libzmq",
    ],
)

cmake_external(
    name = "msgpack",
    cache_entries = {
        "MSGPACK_CXX17": "ON",
    },
    lib_source = "@msgpack//:all",
    static_libraries = ["libmsgpackc.a"],
)

cmake_external(
    name = "natsc",
    # Values to be passed as -Dkey=value on the CMake command line;
    # here are serving to provide some CMake script configuration options
    cache_entries = {
        "NATS_BUILD_STREAMING": "OFF",
    },
    lib_source = "@natsc//:all",
    static_libraries = ["libnats_static.a"],
)

cc_library(
    name = "autobahn",
    srcs = glob(
        [
            "src/**/*.cpp",
            "src/**/*.ipp",
        ],
        exclude = [
            "src/autobahn-plugin.cpp",
            "src/autobahn-server.cpp",
        ],
    ),
    hdrs = glob([
        "include/**/*.hpp",
        "src/**/*.hpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":cppzmq",
        ":libzmq",
        ":msgpack",
        ":natsc",
        "@boost//:algorithm",
        "@boost//:asio",
        "@boost//:process",
    ],
)

cc_binary(
    name = "autobahn-client",
    srcs = ["src/autobahn-client.cpp"],
    includes = [":autobahn/include"],
    linkopts = [
        "-lcrypto",
        "-lssl",
    ],
    deps = [
        "//autobahn",
        # "@boost//:asio",
    ],
)

cc_binary(
    name = "autobahn-server",
    srcs = ["src/autobahn-server.cpp"],
    linkopts = [
        "-lcrypto",
        "-lssl",
    ],
    deps = [
        "//autobahn",
        # "@boost//:asio",
    ],
)

cc_binary(
    name = "autobahn-plugin.so",
    srcs = ["src/autobahn-plugin.cpp"],
    linkopts = [
        "-lcrypto",
        "-lssl",
    ],
    linkshared = 1,
    linkstatic = 1,
    deps = [
        "//autobahn",
    ],
)

cc_test(
    name = "tests",
    srcs = glob(["tests/**/*.cpp"]),
    includes = ["@autobahn:autobahn/include"],
    deps = [
        "//autobahn",
        "@googletest//:gtest_main",
    ],
)
