cmake_minimum_required(VERSION 3.28)
project(Assignment4)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        ../main.cpp
        ../caesar.cpp
)
set(EXEC_SOURCES main.cpp)
add_library(libcaesar SHARED ${SOURCES})
add_executable(Assignment4 ${EXEC_SOURCES})