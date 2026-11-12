add_library(database_connector_system
    src/database_system_connector/models/request.cpp
    src/database_system_connector/models/response.cpp
)

target_include_directories(database_connector_system
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(database_connector_system INTERFACE Boost::json)
