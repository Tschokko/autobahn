load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "tests",
    srcs = glob(["**/*.cpp"]),
    deps = [
        "//autobahn",
        "@googletest//:gtest_main",
    ],
)
