/**
 * @file communication.cpp
 * @brief Implementation of the Communication class for PLC (Programmable Logic Controller) communication.
 * @author Sawada
 * @date 2026-10-21
 */

#include "plc/communication.hpp"

#include <time.h>
#include <assert.h>

#include <cerrno>

Communication::Communication(ProtocolType protocol_type, const std::string& device_name, const bool append_checksum_flag)
    : protocol_type_(protocol_type), device_name_(device_name), append_checksum_flag_(append_checksum_flag) {
    // Initialize the protocol handler based on the selected protocol type
    switch (protocol_type_) {
        case ProtocolType::ComputerLinkTypeNoCRLF:
            protocol_handler_ = new protocol::ComputerLink(device_name_, append_checksum_flag_, protocol_type_ == ProtocolType::ComputerLinkTypeWithCRLF);

            break;
        case ProtocolType::ComputerLinkTypeWithCRLF:
            protocol_handler_ = new protocol::ComputerLink(device_name_, append_checksum_flag_, protocol_type_ == ProtocolType::ComputerLinkTypeWithCRLF);

            break;
        case ProtocolType::ModbusRTU:
            assert(false && "未実装");

            protocol_handler_ = nullptr;

            break;
        default:
            assert(false && "Unsupported protocol type");

            break;
    }