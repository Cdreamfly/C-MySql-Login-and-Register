cmake_minimum_required(VERSION 3.17)
project(untitled)

set(INC_DIR /usr/include/mysql/)
set(LINK_DIR /usr/lib/x86_64-linux-gnu/)
include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(mysqlclient)
set(CMAKE_CXX_STANDARD 17)

add_executable(untitled main.cpp)
