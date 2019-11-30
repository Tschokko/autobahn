load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "autobahn",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["src/**/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = ["@boost//:asio"],
)

cc_binary(
    name = "autobahn-plugin",
    srcs = ["src/main.cpp"],
    deps = [
        "//autobahn",
        "@boost//:asio",
    ],
)
