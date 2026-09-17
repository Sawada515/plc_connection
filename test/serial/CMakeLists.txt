# CMakeLists.txt for serial test

cmake_minimum_required(VERSION 3.10)
project(serial_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(serial_test
    serial_test.cpp
    ../../src/plc/serial/serial.cpp
)

target_include_directories(serial_test
    PRIVATE
        ../../include/plc/serial
)
