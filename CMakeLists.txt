cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

set(PACKAGE_NAME    "clob")
project(${PACKAGE_NAME} CXX)

#-------------------------------------------------------------------------------------------------------
# Resolve version
#-------------------------------------------------------------------------------------------------------
if (NOT DEFINED PACKAGE_VERSION)
    set(PACKAGE_VERSION "0.1.0")
endif()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
if (NOT CMAKE_CXX_STANDARD)
    # If CMAKE_CXX_STANDARD is not set, configure it to 23 by default
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (CMAKE_CXX_STANDARD LESS 23)
    # If CMAKE_CXX_STANDARD is set but less than 23, issue a fatal error
    message(FATAL_ERROR "Clob requires at least C++23. Please set CMAKE_CXX_STANDARD to 23 or higher.")
endif ()

set(PACKAGE_STRING    "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/lunathanael/clob/issues")

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create executable
add_executable(${PACKAGE_NAME} src/main.cpp)

# Add include directories
target_include_directories(${PACKAGE_NAME} PRIVATE include)

set(HEADER_FILES
    include/clob/Market.h
    include/clob/Order.h
    include/clob/OrderBook.h
    include/clob/Stock.h
    include/clob/types.h
)

set(SOURCE_FILES
    src/Market.cpp
)

target_sources(${PACKAGE_NAME} PRIVATE ${SOURCE_FILES})
