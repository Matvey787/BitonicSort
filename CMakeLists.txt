cmake_minimum_required(VERSION 3.11)
project(BitonicSort CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------- Bitonic sort library --------------------

find_package(OpenCL REQUIRED)

add_subdirectory(bs_lib/)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/cxxopts/include/cxxopts.hpp")
    message(FATAL_ERROR 
        " The cxxopts submodule is missing! \n"
        "Please run the following commands to fetch it:\n"
        "  git submodule update --init --recursive\n"
        "----------------------------------------")
endif()

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