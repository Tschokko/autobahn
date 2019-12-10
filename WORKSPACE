load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

http_archive(
    name = "rules_foreign_cc",
    strip_prefix = "rules_foreign_cc-master",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest",
    tag = "release-1.8.1",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
    remote = "https://github.com/nelhage/rules_boost",
    shallow_since = "1570056263 -0700",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

http_archive(
    name = "libzmq",
    build_file_content = all_content,
    strip_prefix = "zeromq-4.3.2",
    urls = ["https://github.com/zeromq/libzmq/releases/download/v4.3.2/zeromq-4.3.2.tar.gz"],
)

http_archive(
    name = "cppzmq",
    build_file_content = all_content,
    strip_prefix = "cppzmq-4.5.0",
    urls = ["https://github.com/zeromq/cppzmq/archive/v4.5.0.tar.gz"],
)

http_archive(
    name = "msgpack",
    build_file_content = all_content,
    strip_prefix = "msgpack-3.2.0",
    urls = ["https://github.com/msgpack/msgpack-c/releases/download/cpp-3.2.0/msgpack-3.2.0.tar.gz"],
)

http_archive(
    name = "natsc",
    build_file_content = all_content,
    strip_prefix = "nats.c-2.1.0",
    urls = ["https://github.com/nats-io/nats.c/archive/v2.1.0.tar.gz"],
)
