#!/bin/bash

python --version

pwd
sed -i '' 's/".so"/".dylib"/' tensorflow_text/tftext.bzl
perl -pi -e "s/(load_library.load_op_library.*)\\.so'/\$1.dylib'/" $(find tensorflow_text/python -type f)

export CC_OPT_FLAGS='-mavx'
./oss_scripts/configure.sh
source ./oss_scripts/install_bazel.sh

pip install numpy

 bazel build \
    --repo_env=CC=gcc \
    --repo_env=CXX=g++ \
    --repo_env=BAZEL_COMPILER=gcc \
    --config=release_arm64_linux \
    oss_scripts/pip_package:build_pip_package
