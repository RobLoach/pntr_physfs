
# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(pntr)
find_package(PhysFS)

# Resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

add_executable(pntr_physfs_examples)
target_sources(pntr_physfs_examples PRIVATE "${CMAKE_CURRENT_LIST_DIR}/pntr_physfs_examples.c")
target_link_libraries(pntr_physfs_examples PRIVATE pntr pntr-physfs physfs-static)
