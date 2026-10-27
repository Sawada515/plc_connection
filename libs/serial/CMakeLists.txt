add_library(serial STATIC
    src/serial.cpp
)

target_include_directories(serial
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(serial PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
