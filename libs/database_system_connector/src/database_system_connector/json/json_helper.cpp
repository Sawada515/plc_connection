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
    )
    {
        if (optional_value.has_value())
        {
            obj[json_key] = boost::json::value_from(*optional_value);
        }
    }

    /**
     * @brief json::objectからstd::optionalに値を取得する（C#側の実装と合わせている）
     * @note json -> struct
     */
    template <typename T>
    std::optional<T> get_optional(
        const boost::json::object& obj,
        std::string_view json_key
    )
    {
        auto it = obj.find(json_key);
        if (it != obj.end() && !it->value().is_null())
        {
            return boost::json::value_to<T>(it->value());
        }

        return std::nullopt;
    }
}
