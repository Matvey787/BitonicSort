cmake_minimum_required(VERSION 3.11)
project(BitonicSort CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------- Bitonic sort library --------------------

add_subdirectory(bs_lib/)
add_subdirectory(src/cxxopts/)

add_executable(biton 
    src/main.cpp
)

target_link_libraries(biton 
    PRIVATE
        bs_lib
        OpenCL
        cxxopts
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(biton PRIVATE DEBUG)
endif()

add_subdirectory(tests/tester/)