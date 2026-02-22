cmake_minimum_required(VERSION 3.28)
project(Tester LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/colors.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/tester_config.cmake)

if(NOT DEFINED CTEST)
    option(CTEST "Enable ctest generation" OFF)
endif()


if(NOT DEFINED GTEST)
    option(GTEST "Enable gtest generation" OFF)
endif()

print_colored_message("    Added tester: ${TESTER_RELATIVE_PATH}    " ${Color_Yellow} ${Bg_Black})

if (CTEST OR GTEST)
    print_colored_message("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓" ${Color_Black} ${Bg_Bright_Green})
    print_colored_message("┃ NOTE: CHANGE TESTER CONFIGURATION tester/tester_config.cmake ┃" ${Color_Black} ${Bg_Bright_Green})
    print_colored_message("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛" ${Color_Black} ${Bg_Bright_Green})
endif()

if (CTEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ctest.cmake)
endif()

if (GTEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gtest.cmake)
endif()