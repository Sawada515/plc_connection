/**
 * @file request.cpp
 * @brief Request model implementation
 * @author Sawada
 * @date 2026-11-12
 */

#include "database_system_connector/models/request.hpp"

#include <boost/json.hpp>

#include <cassert>

namespace database_system_connector::models {
    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const QueryType& query_type
    )
    {
        switch (query_type) {
            case QueryType::Create:
                json_value = "create";
                break;
            case QueryType::Read:
                json_value = "read";
                break;
            case QueryType::Update:
                json_value = "update";
                break;
            case QueryType::Delete:
                json_value = "delete";
                break;
            case QueryType::UserList:
                json_value = "get_user_list";
                break;
            default:
                assert(false && "Invalid QueryType");
        }
    }

    QueryType tag_invoke(
        boost::json::value_to_tag<QueryType>,
        const boost::json::value& json_value
    )
    {
        const std::string& query_type_str = json_value.as_string().c_str();
        if (query_type_str == "create") {
            return QueryType::Create;
        }
        else if (query_type_str == "read") {
            return QueryType::Read;
        }
        else if (query_type_str == "update") {
            return QueryType::Update;
        }
        else if (query_type_str == "delete") {
            return QueryType::Delete;
        }
        else if (query_type_str == "get_user_list") {
            return QueryType::UserList;
        }
        else {
            assert(false && "Invalid QueryType string");
        }
    }

    void tag_invoke(
        boost::json::value_from_tag,
        boost::json::value& json_value,
        const Resource& resource
    )
    {
        switch (resource) {
            case Resource::UserService:
                json_value = "user_service";
                break;
            case Resource::InspectionService:
                json_value = "inspection_service";
                break;
            case Resource::InspectionRequestService:
                json_value = "inspection_request_service";
                break;
            case Resource::StoreService:
                json_value = "store_service";
                break;
            case Resource::DefectService:
                json_value = "defect_service";
                break;
            case Resource::DefectTypeService:
                json_value = "defect_type_service";
                break;
            case Resource::ImageService:
                json_value = "image_service";
                break;
            case Resource::ComplexService:
                json_value = "complex_service";
                break;
            default:
                assert(false && "Invalid Resource");
        }

    }

    Resource tag_invoke(
        boost::json::value_to_tag<Resource>,
        const boost::json::value& json_value
    )
    {
        const std::string_view& resource_str = json_value.as_string().c_str();

        if (resource_str == "user_service") {
            return Resource::UserService;
        }
        else if (resource_str == "inspection_service") {
            return Resource::InspectionService;
        }
        else if (resource_str == "inspection_request_service") {
            return Resource::InspectionRequestService;
        }
        else if (resource_str == "store_service") {
            return Resource::StoreService;
        }
        else if (resource_str == "defect_service") {
            return Resource::DefectService;
        }
        else if (resource_str == "defect_type_service") {
            return Resource::DefectTypeService;
        }
        else if (resource_str == "image_service") {
            return Resource::ImageService;
        }
        else if (resource_str == "complex_service") {
            return Resource::ComplexService;
        }
        else {
            assert(false && "Invalid Resource string");
        }
    }

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