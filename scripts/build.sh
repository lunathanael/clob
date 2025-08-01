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
NO_EXCEPTIONS=false
BUILD_TESTS=false
SANITIZE_ADDRESS=false
SANITIZE_THREAD=false
CODE_COVERAGE=false
USE_VALGRIND=false
VERBOSE_MAKEFILE=false
CLEAN_BUILD=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --no-exceptions)
            NO_EXCEPTIONS=true
            shift
            ;;
        --tests)
            BUILD_TESTS=true
            shift
            ;;
        --sanitize-address)
            SANITIZE_ADDRESS=true
            shift
            ;;
        --sanitize-thread)
            SANITIZE_THREAD=true
            shift
            ;;
        --coverage)
            CODE_COVERAGE=true
            shift
            ;;
        --valgrind)
            USE_VALGRIND=true
            shift
            ;;
        --verbose)
            VERBOSE_MAKEFILE=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
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
            echo "Options: --no-exceptions, --tests, --sanitize-address, --sanitize-thread, --coverage, --valgrind, --verbose, --clean"
            exit 1
            ;;
    esac
done

echo "Building CLOB with build type: $BUILD_TYPE"
if [ "$NO_EXCEPTIONS" = true ]; then
    echo "No exceptions mode: enabled"
fi
if [ "$BUILD_TESTS" = true ]; then
    echo "Tests: enabled"
else
    echo "Tests: disabled"
fi
if [ "$SANITIZE_ADDRESS" = true ]; then
    echo "AddressSanitizer: enabled"
fi
if [ "$SANITIZE_THREAD" = true ]; then
    echo "ThreadSanitizer: enabled"
fi
if [ "$CODE_COVERAGE" = true ]; then
    echo "Code coverage: enabled"
fi
if [ "$USE_VALGRIND" = true ]; then
    echo "Valgrind: enabled"
fi
if [ "$VERBOSE_MAKEFILE" = true ]; then
    echo "Verbose make: enabled"
fi

BUILD_DIR="out"

if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"

echo "Configuring with CMAKE_BUILD_TYPE=$BUILD_TYPE..."

# Configure
CMAKE_ARGS=("-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
if [ "$BUILD_TESTS" = true ]; then
    CMAKE_ARGS+=("-DCLOB_BUILD_TESTS=ON")
else
    CMAKE_ARGS+=("-DCLOB_BUILD_TESTS=OFF")
fi
if [ "$NO_EXCEPTIONS" = true ]; then
    CMAKE_ARGS+=("-DCLOB_NO_EXCEPTIONS=ON")
fi
if [ "$SANITIZE_ADDRESS" = true ]; then
    CMAKE_ARGS+=("-DCLOB_SANITIZE_ADDRESS=ON")
fi
if [ "$SANITIZE_THREAD" = true ]; then
    CMAKE_ARGS+=("-DCLOB_SANITIZE_THREAD=ON")
fi
if [ "$CODE_COVERAGE" = true ]; then
    CMAKE_ARGS+=("-DCLOB_CODE_COVERAGE=ON")
fi
if [ "$USE_VALGRIND" = true ]; then
    CMAKE_ARGS+=("-DCLOB_USE_VALGRIND=ON")
fi
if [ "$VERBOSE_MAKEFILE" = true ]; then
    CMAKE_ARGS+=("-DCLOB_VERBOSE_MAKEFILE=ON")
fi


cmake -S . -B "$BUILD_DIR" "${CMAKE_ARGS[@]}"

echo "Building..."

# Build
if [ "$VERBOSE_MAKEFILE" = true ]; then
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
    if [ "$NO_EXCEPTIONS" = true ]; then
        echo "Exceptions: disabled"
    else
        echo "Exceptions: enabled"
    fi
    if [ "$BUILD_TESTS" = true ]; then
        echo "Tests: built"
    else
        echo "Tests: not built"
    fi
    if [ "$SANITIZE_ADDRESS" = true ]; then
        echo "AddressSanitizer: enabled"
    fi
    if [ "$SANITIZE_THREAD" = true ]; then
        echo "ThreadSanitizer: enabled"
    fi
    if [ "$CODE_COVERAGE" = true ]; then
        echo "Code coverage: enabled"
    fi
    if [ "$USE_VALGRIND" = true ]; then
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
if [ "$BUILD_TESTS" = true ]; then
    echo "Running tests..."
    cd "$BUILD_DIR"
    ctest --output-on-failure
    cd ..
fi