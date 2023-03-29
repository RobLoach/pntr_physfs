
# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(pntr)
find_package(PhysFS)

# Resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# pntr-physfs-test
add_executable(pntr-physfs-test pntr-physfs-test.c)
target_link_libraries(pntr-physfs-test PUBLIC
    pntr
    pntr-physfs
    physfs-static
)

# Set up the test
add_test(NAME pntr-physfs-test COMMAND pntr-physfs-test)
