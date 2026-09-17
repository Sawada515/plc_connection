/**
 * @file protocol.hpp
 * @author sawada
 * @brief PLC Protocol header file
 * @date 2026-09-18
 */

#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include "serial/serial.hpp"

#include <string>
#include <cstring>

namespace plc {
    class Protocol {
      public:
        struct CommonArgs {
            unsigned char branch_number;
            unsigned char pc_number;
            unsigned char teregram_wait;
        };
    
        /**
         * @brief Construct a new Protocol object
         */
        Protocol(Serial *serial, bool append_checksum_flag, bool append_crlf_flag);
    
        /**
         * @brief Destroy the Protocol object
         */
        ~Protocol();

        bool br(
            CommonArgs common_args,
            std::string head_device_number,
            unsigned char number_of_device,
        );
    };
}

#endif  // PROTOCOL_HPP_