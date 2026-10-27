add_executable(plc_app
    main.cpp
)

target_compile_options(plc_app PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(plc_app PRIVATE
    serial
)
