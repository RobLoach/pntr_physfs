cmake_minimum_required(VERSION 3.11)
project(pntr-physfs
    DESCRIPTION "pntr-physfs: PhysicsFS support for pntr"
    HOMEPAGE_URL "https://github.com/robloach/pntr-physfs"
    VERSION 0.0.1
    LANGUAGES C
)

# pntr-physfs
add_library(pntr-physfs INTERFACE)
target_include_directories(pntr-physfs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_PHYSFS_IS_MAIN TRUE)
else()
    set(PNTR_PHYSFS_IS_MAIN FALSE)
endif()
option(PNTR_PHYSFS_BUILD_EXAMPLES "Examples" ${PNTR_PHYSFS_IS_MAIN})

# Examples
if (PNTR_PHYSFS_BUILD_EXAMPLES)
    add_subdirectory(examples)

    # Testing
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        # set(CTEST_CUSTOM_TESTS_IGNORE
        #     pkg-config--static
        # )
        # Always print verbose output when tests fail if run using `make test`.
        #list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
        #add_subdirectory(test)
    endif()
endif()
