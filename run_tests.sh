#/bin/sh
bazel test --cxxopt='-std=c++17' --test_output=all //autobahn:tests
