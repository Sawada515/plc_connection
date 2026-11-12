/**
 * @file request.hpp
 * @brief Request model
 * @author Sawada
 * @date 2026-11-12
 */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "database_system_connector/json/json_helper.hpp"

#include <boost/json.hpp>

#include <string>

namespace database_system_connector::models {
    enum class QueryType {
        Create,
        Read,
        Update,
        Delete,
        UserList
    };

    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const QueryType& query_type
    );

    QueryType tag_invoke(
        boost::json::value_to_tag<QueryType>,
        const boost::json::value& json_value
    );

    enum class Resource {
        UserService,
        InspectionService,
        InspectionRequestService,
        StoreService,
        DefectService,
        DefectTypeService,
        ImageService,
        ComplexService
    };

    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const Resource& resource
    );

    Resource tag_invoke(
        boost::json::value_to_tag<Resource>,
        const boost::json::value& json_value
    );

    template <typename TData>
    struct DatabasePayload {
        QueryType query_type;
        Resource resource;
        TData data;
    };

    template <typename TData>
    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const DatabasePayload<TData>& payload
    )
    {
        json_value = {
            {"query_type", payload.query_type},
            {"resource", payload.resource},
            {"data", boost::json::value_from(payload.data)}
        };
    }

    template <typename TData>
    struct DatabaseRequest {
        int request_id;
        DatabasePayload<TData> payload;

        DatabaseRequest() = defalut;
        DatabaseRequest(
            int request_id,
            QueryType query_type,
            Resource resource,
            const TData& data
        )
        : request_id(request_id)
        , payload{query_type, resource, data}
        {
            ;
        }
    };

    template <typename TData>
    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const DatabaseRequest<TData>& request
    )
    {
        json_value = {
            {"request_id", request.request_id},
            {"payload", boost::json::value_from(request.payload)}
        };
    }
}

#endif // REQUEST_HPP