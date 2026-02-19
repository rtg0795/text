#!/bin/bash

export CC=/usr/lib/llvm-18/bin/clang
export CXX=/usr/lib/llvm-18/bin/clang++
export BAZEL_CXXOPTS="-stdlib=libc++"

pip install --upgrade pip setuptools
pip install --upgrade pip twine
pip install --upgrade pip auditwheel
pip install --upgrade patchelf
pip install --upgrade requests

pip list

source ./oss_scripts/configure.sh

echo "SHARED_LIBRARY_NAME" = "$SHARED_LIBRARY_NAME"

bazel build --config=release_arm64_linux oss_scripts/pip_package:build_pip_package
