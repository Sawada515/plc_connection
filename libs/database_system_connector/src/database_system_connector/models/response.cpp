/**
 * @file response.cpp
 * @brief Response model implementation
 * @author Sawada
 * @date 2026-11-12
 */

#include "database_system_connector/models/response.hpp"

#include "database_system_connector/json/json_helper.hpp"

#include <boost/json.hpp>
#include <optional>
#include <cassert>
#include <string>

namespace database_system_connector::models {
    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const ResponseStatus status
    )
    {
        json_value = (status == ResponseStatus::Success) ? "success" : "error";
    }

    ResponseStatus tag_invoke(
        boost::json::value_to_tag<ResponseStatus>,
        const boost::json::value& json_value
    )
    {
        const std::string& status_str = json_value.as_string().c_str();

        if (status_str == "success") {
            return ResponseStatus::Success;
        }
        else if (status_str == "error") {
            return ResponseStatus::Error;
        }
        else {
            assert(false && "Invalid ResponseStatus");
        }
    }

    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const DatabaseError& error
    )
    {
        boost::json::object obj;

        json::set_optional(obj, "error_code", error.error_code);
        json::set_optional(obj, "message", error.message);

        json_value = std::move(obj);
    }

    DatabaseError tag_invoke(
        boost::json::value_to_tag<DatabaseError>,
        const boost::json::value& json_value
    )
    {
        const auto& obj = json_value.as_object();

        return DatabaseError{
            json::get_optional<int>(obj, "error_code"),
            json::get_optional<std::string>(obj, "message")
        };
    }
}