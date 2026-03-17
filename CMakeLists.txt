cmake_minimum_required(VERSION 3.28)

project(BitonicSort
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/debug_release.cmake)


# -------------------- Bitonic sort library --------------------

find_package(OpenCL REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_subdirectory(bs_lib/)

add_executable(biton
    src/main.cpp
)

target_sources(biton
    PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS src
        FILES
            src/commandLineHandler.cppm
)

set_target_properties(biton PROPERTIES
    CXX_SCAN_FOR_MODULES ON
)

target_link_libraries(biton
    PRIVATE
        bs_lib
        OpenCL
        LLVMCore
        LLVMSupport
)

target_include_directories(biton
    PRIVATE
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(biton
    PRIVATE
        ${LLVM_DEFINITIONS}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(biton PRIVATE DEBUG)
endif()

add_subdirectory(tests/tester/)