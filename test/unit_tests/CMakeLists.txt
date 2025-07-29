function(clob_add_test TEST_NAME SOURCES)
    set(HEADER_FILES
            ../bundled/doctest/doctest.h
            ../misc/TestUtilities.h
    )

    set(ADD_SOURCE_FILES
            ../misc/TestMain.cpp
            ../misc/TestUtilities.cpp)

    list(APPEND SOURCES ${ADD_SOURCE_FILES})

    # Create a test executable
    add_executable(${TEST_NAME} "")

    set_common_compile_options(${TEST_NAME})

    # Add sources
    target_sources(${TEST_NAME} PRIVATE ${SOURCES} ${HEADER_FILES})

    # include dirs
    target_include_directories(${TEST_NAME}
            PUBLIC
            ${PROJECT_SOURCE_DIR}/test/misc
            ${PROJECT_SOURCE_DIR}/test/bundled
            PRIVATE
            ..)

    # Link dependencies
    target_link_libraries(${TEST_NAME} ${LIBRARY_NAME})

    # Do not decay cxx standard if not specified
    set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

    # Set output test directory
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/test)

    # Add this target to the post build unit tests
    doctest_discover_tests(${TEST_NAME})
endfunction()

include(Doctest)

clob_add_test(TEST_MarketTest MarketTest.cpp)
clob_add_test(TEST_StockTest StockTest.cpp)
clob_add_test(TEST_LimitOrderTest LimitOrder.cpp)
clob_add_test(TEST_OrderBookTest OrderBookTest.cpp)