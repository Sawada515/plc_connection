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

#include <cstdint>
#include <string>
#include <cstring>
#include <vector>

namespace protocol {
    class ComputerLink {
      public:
    
        /**
         * @brief Construct a new ComputerLink object
         */
        ComputerLink(const std::string& device_name, bool append_checksum_flag, bool append_crlf_flag);
    
        /**
         * @brief Destroy the ComputerLink object
         */
        ~ComputerLink();

        bool br(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const std::
        );

        bool wr(
        );

        bool qr(
        ); 
        

      private:
        enum class CommandType : std::uint16_t {
            BR,
            WR,
            QR,
            BW,
            WW,
            QW,
            
        }

        /**
         * @brief Calculate the checksum of the given data
         * 
         * @param[in] data Pointer to the data
         * @param[in] length Length of the data
         * @return std::uint16 The calculated checksum
         */
        std::uint16_t calculate_checksum(const std::uint8_t* data, size_t length);
    
        /**
         * @brief Wait for a specified number of milliseconds
         * 
         * @param[in] ms Number of milliseconds to wait
         * @return true if the wait was successful
         * @return false if an error occurred during the wait
         */
        bool wait_ms(long ms);

        bool write(const std::uint8_t *data, size_t size);
        bool read(vector<std::uint8_t>& buffer);

        std::string device_name_;
    };
}

#endif  // COMPUTER_LINK_HPP_