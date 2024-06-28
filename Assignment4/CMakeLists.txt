cmake_minimum_required(VERSION 3.28)
project(Assignment4)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES ../caesar.cpp)
add_library(libcaesar SHARED ${SOURCES})

target_include_directories(libcaesar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(EXEC_SOURCES main.cpp)

add_executable(Assignment4 ${EXEC_SOURCES})

set_target_properties(libcaesar PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../libcaesar"
)

set_target_properties(Assignment4 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)
