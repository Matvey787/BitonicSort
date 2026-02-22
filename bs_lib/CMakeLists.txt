set(BS_LIB bs_lib)
add_library(${BS_LIB} INTERFACE)  


target_include_directories(${BS_LIB} 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)


