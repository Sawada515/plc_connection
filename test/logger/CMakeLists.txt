add_executable(logger_test
    logger_test.cpp
)

target_compile_options(logger_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(logger_test PRIVATE
    logger
)
