cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(TARGET_NAME    "clob")
set(LIBRARY_NAME    "clob_lib")
project(${TARGET_NAME} CXX)

#-------------------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------------------

option(CLOB_NO_EXCEPTIONS "Enable this option to build without exception handling support." OFF)
option(CLOB_BUILD_TESTS "Enable this option to build the test suite." OFF)
option(CLOB_SANITIZE_ADDRESS "Enable AddressSanitizer (ASan) for memory error detection." OFF)
option(CLOB_SANITIZE_THREAD "Enable ThreadSanitizer (TSan) for detecting thread-related issues. Note: Using this option with non-Clang compilers may produce false positives." OFF)
option(CLOB_CODE_COVERAGE "Enable code coverage analysis during the build." OFF)
option(CLOB_USE_VALGRIND "Use Valgrind as the default memory checking tool in CTest. Valgrind must be installed." OFF)

# Internal option
set(CLOB_ENABLE_GCC_HARDENING OFF CACHE INTERNAL "")

#-------------------------------------------------------------------------------------------------------
# Determine if clob is built as a subproject (using add_subdirectory) or if it is the master project.
#-------------------------------------------------------------------------------------------------------
set(CLOB_MASTER_PROJECT FALSE CACHE BOOL "Master Project" FORCE)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CLOB_MASTER_PROJECT TRUE CACHE BOOL "Master Project" FORCE)
endif ()

#-------------------------------------------------------------------------------------------------------
# Custom cmake functions
#-------------------------------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ClobUtils)

#-------------------------------------------------------------------------------------------------------
# Resolve version
#-------------------------------------------------------------------------------------------------------
clob_extract_version()
project(clob VERSION ${CLOB_VERSION} LANGUAGES CXX)

#-------------------------------------------------------------------------------------------------------
# Set default build to release
#-------------------------------------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif ()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#-------------------------------------------------------------------------------------------------------
# Required Packages
#-------------------------------------------------------------------------------------------------------
# find_package(Threads REQUIRED)

if (CLOB_BUILD_TESTS)
    enable_testing()

    if (CLOB_USE_VALGRIND)
        # find valgrind
        find_program(MEMORYCHECK_COMMAND NAMES valgrind)
        if (NOT MEMORYCHECK_COMMAND)
            message(WARNING "Valgrind not found")
        endif ()

        # set valgrind params
        set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --leak-resolution=med --show-leak-kinds=all --track-origins=yes --vgdb=no --fair-sched=yes")

        # add memcheck test action to ctest
        include(CTest)
    endif ()
endif ()

#-------------------------------------------------------------------------------------------------------
# Log Info
#-------------------------------------------------------------------------------------------------------
if (CLOB_MASTER_PROJECT)
    option(CLOB_VERBOSE_MAKEFILE "Enable verbose output for makefiles when Clob is the master project. This provides detailed information about the build process." OFF)
    message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
    message(STATUS "CLOB_VERSION: ${CLOB_VERSION}")
endif ()

message(STATUS "CLOB_NO_EXCEPTIONS: " ${CLOB_NO_EXCEPTIONS})

#---------------------------------------------------------------------------------------
# Verbose make file option
#---------------------------------------------------------------------------------------
if (CLOB_VERBOSE_MAKEFILE)
    set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "Verbose output" FORCE)
endif ()

# address sanitizer flags
if (CLOB_SANITIZE_ADDRESS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address,undefined)
endif ()

# thread sanitizer flags
if (CLOB_SANITIZE_THREAD)
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=thread)
endif ()

# Append extra options for coverage
if (CLOB_CODE_COVERAGE)
    add_compile_options(-O0 -fno-omit-frame-pointer -g -fprofile-arcs -ftest-coverage)
    add_link_options(-fprofile-arcs -ftest-coverage)
endif ()

if (CLOB_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
    add_subdirectory(test)
endif ()

# header files
set(HEADER_FILES
    include/clob/LimitOrder.h
    include/clob/Market.h
    include/clob/OrderBook.h
    include/clob/Stock.h
    include/clob/types.h
    include/clob/version.h
)

set(SOURCE_FILES
    src/Market.cpp
    src/OrderBook.cpp
)

add_library(${LIBRARY_NAME} OBJECT ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_include_directories(${LIBRARY_NAME} PUBLIC protos)
target_include_directories(${LIBRARY_NAME} PRIVATE /home/lunat/.local/include)

# Apply common compile options to object library
set_common_compile_options(${LIBRARY_NAME})

# Apply build type optimizations using modern CMake
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${LIBRARY_NAME} PRIVATE -O0 -g -DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${LIBRARY_NAME} PRIVATE -O3 -DNDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Performance")
    target_compile_options(${LIBRARY_NAME} PRIVATE -Ofast -march=native -ffast-math -flto -DNDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(${LIBRARY_NAME} PRIVATE -O2 -g -DNDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_options(${LIBRARY_NAME} PRIVATE -Os -DNDEBUG)
endif ()

# Apply no-exceptions option (independent of build type)
if (CLOB_NO_EXCEPTIONS)
    target_compile_options(${LIBRARY_NAME} PRIVATE -fno-exceptions -fno-rtti)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DCLOB_NO_EXCEPTIONS)
endif ()

#-------------------------------------------------------------------------------------------------------
# gRPC
#-------------------------------------------------------------------------------------------------------
include(./cmake/GRPC.cmake)
find_package(absl CONFIG REQUIRED)

# Proto file
get_filename_component(market_proto "./protos/market.proto" ABSOLUTE)
get_filename_component(market_proto_path "${market_proto}" PATH)

# Generated sources
set(market_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/market.pb.cc")
set(market_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/market.pb.h")
set(market_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/market.grpc.pb.cc")
set(market_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/market.grpc.pb.h")
add_custom_command(
      OUTPUT "${market_proto_srcs}" "${market_proto_hdrs}" "${market_grpc_srcs}" "${market_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${market_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${market_proto}"
      DEPENDS "${market_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(market_grpc_proto
  ${market_grpc_srcs}
  ${market_grpc_hdrs}
  ${market_proto_srcs}
  ${market_proto_hdrs})
target_link_libraries(market_grpc_proto
  absl::absl_log
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

#-------------------------------------------------------------------------------------------------------
# Executable
#-------------------------------------------------------------------------------------------------------'
add_executable(${TARGET_NAME} src/MarketMakerServer.cpp)
target_link_libraries(${TARGET_NAME}
    PRIVATE
    ${LIBRARY_NAME}
    market_grpc_proto
    absl::flags_parse
    absl::absl_log
    absl::log_initialize
    absl::log_globals
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    ssl crypto)

# Apply build type optimizations to executable
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${TARGET_NAME} PRIVATE -O0 -g -DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${TARGET_NAME} PRIVATE -O3 -DNDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Performance")
    target_compile_options(${TARGET_NAME} PRIVATE -Ofast -march=native -ffast-math -flto -DNDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(${TARGET_NAME} PRIVATE -O2 -g -DNDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_options(${TARGET_NAME} PRIVATE -Os -DNDEBUG)
endif ()

# Apply no-exceptions option to executable (independent of build type)
if (CLOB_NO_EXCEPTIONS)
    target_compile_options(${TARGET_NAME} PRIVATE -fno-exceptions -fno-rtti)
    target_compile_definitions(${TARGET_NAME} PRIVATE -DCLOB_NO_EXCEPTIONS)
endif ()
