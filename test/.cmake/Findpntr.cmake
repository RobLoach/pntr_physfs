# pntr
include(FetchContent)
# DOWNLOAD_EXTRACT_TIMESTAMP is only understood by CMake 3.24 and later.
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    set(PNTR_DOWNLOAD_EXTRACT_TIMESTAMP DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
endif()
FetchContent_Declare(
    pntr
    URL https://github.com/RobLoach/pntr/archive/refs/tags/v0.1.0.zip
    ${PNTR_DOWNLOAD_EXTRACT_TIMESTAMP}
)
FetchContent_MakeAvailable(pntr)
