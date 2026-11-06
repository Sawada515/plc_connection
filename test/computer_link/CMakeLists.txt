add_executable(computer_link_test
    computer_link_test.cpp
)

target_compile_options(computer_link_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(computer_link_test PRIVATE plc)
