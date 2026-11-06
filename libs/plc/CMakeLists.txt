add_library(plc
    src/plc/communication.cpp
    src/protocol/computer_link.cpp
)

target_include_directories(plc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plc PUBLIC serial)
