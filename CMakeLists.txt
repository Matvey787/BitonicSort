cmake_minimum_required(VERSION 3.11)
project(BitonicSort CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------- Bitonic sort library --------------------
set(BS_LIB bs)
add_library(${BS_LIB} INTERFACE)  


target_include_directories(${BS_LIB} 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)





add_executable(biton 
    src/main.cpp
)

target_link_libraries(biton 
    PRIVATE
        ${BS_LIB} 
        OpenCL
)


add_executable(compare 
    src/main.cpp  
)

target_link_libraries(compare 
    PRIVATE
        ${BS_LIB}
        OpenCL
)


target_compile_definitions(compare
    PRIVATE
        COMPARE  
)
