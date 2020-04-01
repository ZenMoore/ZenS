cmake_minimum_required(VERSION 3.14)
project(zens_host_due)

set(CMAKE_CXX_STANDARD 14)

add_executable(zens_host_due main.cpp A_to_demo.h B_to_gestRecog_demo.h none_to_demo.h SerialPort.h SerialPort.cpp)