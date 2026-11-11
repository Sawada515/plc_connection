add_library(uds
    src/uds/client.cpp
    src/uds/server.cpp
    src/uds/common/status.cpp
    src/uds/common/io.cpp
)

target_include_directories(uds
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(uds PUBLIC ASIO_STANDALONE)

find_package(Threads REQUIRED)
target_link_libraries(uds PUBLIC Threads::Threads)
