# pntr

include(FetchContent)
FetchContent_Declare(
    pntr
    GIT_REPOSITORY https://github.com/robloach/pntr.git
    GIT_TAG d23394abafc013f58fbcd8386a7a07501f5c54e0
)
FetchContent_MakeAvailable(pntr)
#include_directories(${pntr_SOURCE_DIR}/src)
