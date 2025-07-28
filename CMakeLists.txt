cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

set(PACKAGE_NAME    "clob")
if (NOT DEFINED PACKAGE_VERSION)
    set(PACKAGE_VERSION "0.1.0")
endif()

set(PACKAGE_STRING    "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/lunathanael/clob/issues")

project(${PACKAGE_NAME} CXX)

add_executable(${PACKAGE_NAME} src/main.cpp)