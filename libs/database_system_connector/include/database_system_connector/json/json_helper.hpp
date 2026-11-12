/**
 * @file json_helper.hpp
 * @brief JSON helper functions
 * @author Sawada
 * @date 2026-11-12
 */

#ifndef JSON_HELPER_HPP
#define JSON_HELPER_HPP

#include <boost/json.hpp>
#include <optional>
#include <string_view>

namespace database_system_connector::json
{
    /**
     * @brief std::optionalに値がある場合のみjson::objectに書き込む（C#側の実装と合わせている）
     * @note struct -> json
     */
    template <typename T>
    void set_optional(
        boost::json::object& obj,
        std::string_view json_key,
        const std::optional<T>& optional_value
    );

    /**
     * @brief json::objectからstd::optionalに値を取得する（C#側の実装と合わせている）
     * @note json -> struct
     */
    template <typename T>
    std::optional<T> get_optional(
        const boost::json::object& obj,
        std::string_view json_key
    );
}

#endif // JSON_HELPER_HPP