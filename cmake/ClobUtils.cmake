# Get Clob version from include/clob/Version.h and store it as CLOB_VERSION
function(clob_extract_version)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/include/clob/version.h" file_contents)

    string(REGEX MATCH "constexpr uint32_t VERSION_MAJOR{([0-9]+)}" _ "${file_contents}")
    if (NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Failed to extract major version number from clob/version.h")
    endif ()
    set(version_major ${CMAKE_MATCH_1})

    string(REGEX MATCH "constexpr uint32_t VERSION_MINOR{([0-9]+)}" _ "${file_contents}")
    if (NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Failed to extract minor version number from clob/version.h")
    endif ()
    set(version_minor ${CMAKE_MATCH_1})

    string(REGEX MATCH "constexpr uint32_t VERSION_PATCH{([0-9]+)}" _ "${file_contents}")
    if (NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Failed to extract patch version number from clob/version.h")
    endif ()
    set(version_patch ${CMAKE_MATCH_1})

    set(CLOB_VERSION "${version_major}.${version_minor}.${version_patch}" PARENT_SCOPE)
endfunction()

# Define the function to set common compile options
function(set_common_compile_options target_name)
    cmake_parse_arguments(COMPILE_OPTIONS "" "VISIBILITY" "" ${ARGN})

    # Set default visibility to PRIVATE if not provided
    if (NOT DEFINED COMPILE_OPTIONS_VISIBILITY)
        set(COMPILE_OPTIONS_VISIBILITY PRIVATE)
    endif ()

    target_compile_options(${target_name} ${COMPILE_OPTIONS_VISIBILITY}
            # GCC-specific hardening and security flags
            $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${CLOB_ENABLE_GCC_HARDENING}>>:
            -fstack-protector-strong
            -fstack-clash-protection
            -Wformat
            -Werror=format-security
            -fcf-protection
            -Wdate-time
            -D_FORTIFY_SOURCE=2
            >
    )

    if (CLOB_NO_EXCEPTIONS)
        # Modify CMake's default flags for MSVC to remove /EHsc
        if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        endif ()

        # Add flags -fno-exceptions -fno-rtti to make sure we support them
        target_compile_options(${target_name} ${COMPILE_OPTIONS_VISIBILITY}
                $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
                -fno-exceptions -fno-rtti>
                $<$<CXX_COMPILER_ID:MSVC>:/wd4702 /GR- /EHs-c- /D_HAS_EXCEPTIONS=0>)
    else ()
        # Additional MSVC specific options - only set for non-CLOB_NO_EXCEPTIONS builds
        target_compile_options(${target_name} ${COMPILE_OPTIONS_VISIBILITY}
                $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)
    endif ()
endfunction()
