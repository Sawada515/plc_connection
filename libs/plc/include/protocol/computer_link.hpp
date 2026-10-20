/**
 * @file computer_link.hpp
 * @author Sawada
 * @brief 
 * @version 0.1
 * @date 2026-09-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef COMPUTER_LINK_HPP_
#define COMPUTER_LINK_HPP_

#include "serial/serial.hpp"

#include <string>
#include <cstring>
#include <vector>

namespace protocol {
    class ComputerLink {
      public:
        struct CommonArgs {
            unsigned char branch_number;
            unsigned char pc_number;
            unsigned char teregram_wait;
        };
    
        /**
         * @brief Construct a new ComputerLink object
         */
        ComputerLink(const std::string& device_name, bool append_checksum_flag, bool append_crlf_flag);
    
        /**
         * @brief Destroy the ComputerLink object
         */
        ~ComputerLink();

        bool br(
            CommonArgs common_args,
            std::string head_device_number,
            unsigned char number_of_device,
            std::vector<unsigned char> data,
        );

        bool wr(
            CommonArgs common_args,
            std::string head_device_number,
            unsigned char number_of_device,
            std::vector<unsigned char> data,
        );

        bool qr(
            CommonArgs common_args,
            std::string head_device_number,
            unsigned char number_of_device,
            std::vector<unsigned char> data,
        ); 
        

      private:
        std::uint16_t calculate_checksum(const std::uint8_t *data, size_t size);

        bool write(const std::uint8_t *data, size_t size);
        bool read(vector<std::uint8_t>& buffer);

        std::string device_name_;
    };
}

#endif  // COMPUTER_LINK_HPP_