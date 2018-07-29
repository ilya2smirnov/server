cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
project(server)
add_executable(${PROJECT_NAME} main.cpp udpechoserver.cpp udpechoserver.h tcpechoserver.cpp tcpechoserver.h mymath.h mymath.cpp)
target_link_libraries(server pthread)



