update-alternatives --install \
  /usr/bin/python3 python3 /usr/bin/python${PY_VERSION} 1
update-alternatives --set python3 /usr/bin/python${PY_VERSION}

/usr/bin/python3 -m pip install --upgrade pip
pip install --upgrade pip setuptools
pip install --upgrade pip twine
pip install --upgrade auditwheel
pip install --upgrade patchelf
pip install --upgrade requests

pip list
source ./oss_scripts/configure.sh
echo "SHARED_LIBRARY_NAME = $SHARED_LIBRARY_NAME"

ARCH=$(uname -m)

case "$ARCH" in
    aarch64)
        echo "Running commands for ARM64 (aarch64)..."
        bazel build --config=release_cpu_linux oss_scripts/pip_package:build_pip_package
        ;;
    x86_64)
        echo "Running commands for x86 64-bit (x86_64)..."
        bazel build --config=release_arm64_linux oss_scripts/pip_package:build_pip_package
        ;;
    *)
        # Fallback for any other architecture (e.g., i386, armv7l)
        echo "Error: Unsupported architecture ($ARCH)."
        exit 1
        ;;
esac

# comment

