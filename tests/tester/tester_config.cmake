# Tester framework configuration

# CTest
set(CTEST_PROGRAM_EXEC   "${CMAKE_CURRENT_SOURCE_DIR}/../../build/compare")
set(CTEST_INPUTS_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/../e2e")
set(CTEST_ANSWERS_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/../e2e")

set(CTEST_INPUT_PATTERN  "dat")
set(CTEST_ANSWER_PATTERN "ans")
set(CTEST_PREFIX         "...")
set(CTEST_TIMEOUT        30)

# Gtest
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../unit")
set(GTEST_OUTPUT_EXEC_FILE_NAME "unit_tests")
set(GTEST_ADDITIONAL_LIBS_PATHS "")
set(GTEST_ADDITIONAL_LIBS_NAMES "bs_lib" "OpenCL")










# Do not change without any special reason
set(CTESTER_SCRIPT       "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tester_script.py")
set(GTEST_FETCH_URL "https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip")

string(REPLACE "${CMAKE_SOURCE_DIR}" "" TESTER_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")