#/bin/sh
bazel run --cxxopt='-std=c++17' --test_output=all //autobahn:autobahn-server
