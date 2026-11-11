add_executable(uds_test
    uds_test.cpp
)

target_compile_options(uds_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(uds_test PRIVATE
    uds
)
