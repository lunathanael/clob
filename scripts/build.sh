#!/bin/bash


# CLOB Build Script
# Usage: ./build.sh [build_type] [options...]
# Build types: Debug, Release, Performance, RelWithDebInfo, MinSizeRel
# Options:
#   --no-exceptions     Disable exception handling
#   --tests             Build test suite
#   --sanitize-address  Enable AddressSanitizer
#   --sanitize-thread   Enable ThreadSanitizer
#   --coverage          Enable code coverage
#   --valgrind          Enable Valgrind for tests
#   --verbose           Enable verbose make output

set -euo pipefail

BUILD_TYPE="Release"
NO_EXCEPTIONS=OFF
BUILD_TESTS=OFF
SANITIZE_ADDRESS=OFF
SANITIZE_THREAD=OFF
CODE_COVERAGE=OFF
USE_VALGRIND=OFF
VERBOSE_MAKEFILE=OFF

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --no-exceptions)
            NO_EXCEPTIONS=ON
            shift
            ;;
        --tests)
            BUILD_TESTS=ON
            shift
            ;;
        --sanitize-address)
            SANITIZE_ADDRESS=ON
            shift
            ;;
        --sanitize-thread)
            SANITIZE_THREAD=ON
            shift
            ;;
        --coverage)
            CODE_COVERAGE=ON
            shift
            ;;
        --valgrind)
            USE_VALGRIND=ON
            shift
            ;;
        --verbose)
            VERBOSE_MAKEFILE=true
            shift
            ;;
        Debug|Release|Performance|RelWithDebInfo|MinSizeRel)
            BUILD_TYPE="$1"
            shift
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: ./build.sh [build_type] [options...]"
            echo "Build types: Debug, Release, Performance, RelWithDebInfo, MinSizeRel"
            echo "Options: --no-exceptions, --tests, --sanitize-address, --sanitize-thread, --coverage, --valgrind, --verbose"
            exit 1
            ;;
    esac
done

echo "Building CLOB with build type: $BUILD_TYPE"
if [ "$NO_EXCEPTIONS" = ON ]; then
    echo "No exceptions mode: enabled"
fi
if [ "$BUILD_TESTS" = ON ]; then
    echo "Tests: enabled"
else
    echo "Tests: disabled"
fi
if [ "$SANITIZE_ADDRESS" = ON ]; then
    echo "AddressSanitizer: enabled"
fi
if [ "$SANITIZE_THREAD" = ON ]; then
    echo "ThreadSanitizer: enabled"
fi
if [ "$CODE_COVERAGE" = ON ]; then
    echo "Code coverage: enabled"
fi
if [ "$USE_VALGRIND" = ON ]; then
    echo "Valgrind: enabled"
fi
if [ "$VERBOSE_MAKEFILE" = ON ]; then
    echo "Verbose make: enabled"
fi

BUILD_DIR="build"

mkdir -p "$BUILD_DIR"

echo "Configuring with CMAKE_BUILD_TYPE=$BUILD_TYPE..."

# Configure
CMAKE_ARGS=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
CMAKE_ARGS+=("-DCLOB_BUILD_TESTS=$BUILD_TESTS")
CMAKE_ARGS+=("-DCLOB_NO_EXCEPTIONS=$NO_EXCEPTIONS")
CMAKE_ARGS+=("-DCLOB_SANITIZE_ADDRESS=$SANITIZE_ADDRESS")
CMAKE_ARGS+=("-DCLOB_SANITIZE_THREAD=$SANITIZE_THREAD")
CMAKE_ARGS+=("-DCLOB_CODE_COVERAGE=$CODE_COVERAGE")
CMAKE_ARGS+=("-DCLOB_USE_VALGRIND=$USE_VALGRIND")
CMAKE_ARGS+=("-DCLOB_VERBOSE_MAKEFILE=$VERBOSE_MAKEFILE")

echo "Configuring with CMAKE_ARGS=${CMAKE_ARGS[@]} -DCLOB_USE_SYSTEM_GRPC=ON"
cmake -S . -B "$BUILD_DIR" "${CMAKE_ARGS[@]}" -DCLOB_USE_SYSTEM_GRPC=ON -DCMAKE_PREFIX_PATH=$HOME/.local

echo "Building..."

# Build
if [ "$VERBOSE_MAKEFILE" = ON ]; then
    cmake --build "$BUILD_DIR" -j"$(nproc)" --verbose
else
    cmake --build "$BUILD_DIR" -j"$(nproc)"
fi

echo "Build complete!"
echo "Binary location: $BUILD_DIR/clob"

# Show optimization info
if [ -f "$BUILD_DIR/clob" ]; then
    echo "Binary size: $(ls -lh "$BUILD_DIR/clob" | awk '{print $5}')"
    echo "Build type: $BUILD_TYPE"
    if [ "$NO_EXCEPTIONS" = ON ]; then
        echo "Exceptions: disabled"
    else
        echo "Exceptions: enabled"
    fi
    if [ "$BUILD_TESTS" = ON ]; then
        echo "Tests: built"
    else
        echo "Tests: not built"
    fi
    if [ "$SANITIZE_ADDRESS" = ON ]; then
        echo "AddressSanitizer: enabled"
    fi
    if [ "$SANITIZE_THREAD" = ON ]; then
        echo "ThreadSanitizer: enabled"
    fi
    if [ "$CODE_COVERAGE" = ON ]; then
        echo "Code coverage: enabled"
    fi
    if [ "$USE_VALGRIND" = ON ]; then
        echo "Valgrind: enabled"
    fi
    
    # Show optimization level
    case $BUILD_TYPE in
        "Debug")
            echo "Optimization: -O0 (no optimization, debug symbols)"
            ;;
        "Release")
            echo "Optimization: -O3 (maximum optimization)"
            ;;
        "Performance")
            echo "Optimization: -Ofast (aggressive optimization with native CPU)"
            ;;
        "RelWithDebInfo")
            echo "Optimization: -O2 (optimized with debug symbols)"
            ;;
        "MinSizeRel")
            echo "Optimization: -Os (size-optimized)"
            ;;
    esac
fi

# Run tests if they were built
if [ "$BUILD_TESTS" = ON ]; then
    echo "Running tests..."
    cd "$BUILD_DIR"
    ctest --output-on-failure
    cd ..
fi