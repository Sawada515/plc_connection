add_executable(serial_test
    serial_test.cpp
)

target_compile_options(serial_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# libs/serial で定義されたターゲットをリンク
target_link_libraries(serial_test PRIVATE
    serial
)
