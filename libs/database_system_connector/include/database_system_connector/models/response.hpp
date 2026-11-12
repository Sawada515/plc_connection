/**
 * @file response.hpp
 * @brief Response model
 * @author Sawada
 * @date 2026-11-12
 */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "database_system_connector/json/json_helper.hpp"

#include <boost/json.hpp>
#include <optional>
#include <cassert>
#include <string>

namespace database_system_connector::models {
    enum class ResponseStatus {
        Success,
        Error
    };

    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const ResponseStatus status
    );

    ResponseStatus tag_invoke(
        boost::json::value_to_tag<ResponseStatus>,
        const boost::json::value& json_value
    );

    struct DatabaseError {
        std::optional<int> error_code;
        std::optional<std::string> message;
    };

    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const DatabaseError& error
    );

    DatabaseError tag_invoke(
        boost::json::value_to_tag<DatabaseError>,
        const boost::json::value& json_value
    );

    template <typename TPayload = boost::json::value>
    struct DatabaseResponse {
        int request_id{};
        ResponseStatus status{ResponseStatus::Error};
        std::optional<TPayload> payload;
        std::optional<DatabaseError> error;

        [[nodiscard]] bool is_success() const {
            return status == ResponseStatus::Success;
        }
    };

    template <typename TPayload>
    DatabaseResponse<TPayload> tag_invoke(
        boost::json::value_to_tag<DatabaseResponse<TPayload>>,
        const boost::json::value& json_value
    );
}

#endif // RESPONSE_HPP