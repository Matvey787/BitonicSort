foreach(lib_path ${GTEST_ADDITIONAL_LIBS_PATHS})
    get_filename_component(lib_name ${lib_path} NAME)
    set(binary_dir ${CMAKE_CURRENT_BINARY_DIR}/${lib_name})
    message(STATUS "Tester has added library: ${lib_path}, binary dir is ${binary_dir}")
    add_subdirectory(${lib_path} ${binary_dir})
endforeach()


enable_testing()

include(FetchContent)

find_package(GTest REQUIRED)

if (NOT GTest_FOUND)
    message(STATUS "Fetching gtest...") 
    FetchContent_Declare(
        googletest
        URL ${GTEST_FETCH_URL}
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

else ()
    message(STATUS "GTEST tests folder: ${GTEST_DIR}")
endif()

file(GLOB GTEST_FILES "${GTEST_DIR}/*.cpp")

if(NOT GTEST_FILES)
    message(FATAL_ERROR "No test source files found in GTEST_DIR: ${GTEST_DIR}")
endif()

add_executable(${GTEST_OUTPUT_EXEC_FILE_NAME} ${GTEST_FILES})

set_target_properties(${GTEST_OUTPUT_EXEC_FILE_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${GTEST_OUTPUT_EXEC_FILE_NAME}
    PRIVATE
    GTest::gtest GTest::gtest_main
    ${GTEST_ADDITIONAL_LIBS_NAMES}
)

print_colored_message("GTests generated succesfully, run it by command: \"./build${TESTER_RELATIVE_PATH}/${GTEST_OUTPUT_EXEC_FILE_NAME}\"" 
    ${Color_Bright_Green} ${Bg_Default}
)
