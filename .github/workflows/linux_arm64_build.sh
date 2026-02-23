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

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18
ls -l /usr/lib/llvm-18/bin/clang
/usr/lib/llvm-18/bin/clang --version

bazel build \
    --config=release_arm64_linux_clang_local \
    oss_scripts/pip_package:build_pip_package \
    --subcommands=pretty_print

