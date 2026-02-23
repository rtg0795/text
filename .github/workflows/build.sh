#!/bin/bash

python --version

pwd
sed -i '' 's/".so"/".dylib"/' tensorflow_text/tftext.bzl
perl -pi -e "s/(load_library.load_op_library.*)\\.so'/\$1.dylib'/" $(find tensorflow_text/python -type f)

export CC_OPT_FLAGS='-mavx'
./oss_scripts/configure.sh
source ./oss_scripts/install_bazel.sh

# Override the clang settings from TensorFlow's .bazelrc
cat >> .bazelrc << 'EOF'
# Override to use GCC for ARM64
build:release_arm64_linux --repo_env=CC=gcc
build:release_arm64_linux --repo_env=CXX=g++
build:release_arm64_linux --repo_env=BAZEL_COMPILER=gcc
EOF

pip install numpy

 bazel build \
    --repo_env=CC=gcc \
    --repo_env=CXX=g++ \
    --repo_env=BAZEL_COMPILER=gcc \
    --config=release_arm64_linux \
    oss_scripts/pip_package:build_pip_package \
    --subcommands=pretty_print
