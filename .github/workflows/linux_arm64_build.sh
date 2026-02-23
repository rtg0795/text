#!/bin/bash
set -x

pip install --upgrade pip setuptools
pip install --upgrade pip twine
pip install --upgrade pip auditwheel
pip install --upgrade patchelf
pip install --upgrade requests

pip list

source ./oss_scripts/configure.sh

echo "SHARED_LIBRARY_NAME" = "$SHARED_LIBRARY_NAME"

gcc --version
which gcc

cat >> .bazelrc << 'EOF'
# Override to use GCC for ARM64
build:release_arm64_linux --repo_env=CC=gcc
build:release_arm64_linux --repo_env=CXX=g++
build:release_arm64_linux --repo_env=BAZEL_COMPILER=gcc
EOF

bazel build --config=release_arm64_linux --copt=-v oss_scripts/pip_package:build_pip_package --subcommands=pretty_print

