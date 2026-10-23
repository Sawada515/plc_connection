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
        ComputerLink(const std::string& device_name, bool append_checksum_flag, bool append_crlf_flag, unsigned int baudrate_key, const int timeout_ms);
    
        /**
         * @brief Destroy the ComputerLink object
         */
        ~ComputerLink();

        bool br(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const sdt::string &device_address,
            const std::uint8_t number_of_devices,
            std::vector<std::uint8_t> &recv_data
        );

        bool wr(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const sdt::string &device_address,
            const std::uint8_t number_of_devices,
            const std::vector<std::uint8_t> &recv_data
        );

        bool qr(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const sdt::string &device_address,
            const std::uint8_t number_of_devices,
            std::vector<std::uint8_t> &recv_data
        ); 

        bool bw(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const std::string &device_address,
            const std::uint8_t number_of_devices,
            std::vector<std::uint16_t> &send_data
        );

        bool ww(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const std::string &device_address,
            const std::uint8_t number_of_devices,
            const std::vector<std::uint16_t> &send_data
        );

        bool qw(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const std::string &device_address,
            const std::uint8_t number_of_devices,
            std::vector<std::uint16_t> &recv_data
        );

        bool bt(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const std::string &device_address,
            const std::uint8_t number_of_devices,
            const std::vector<std::uint32_t> &send_data
        );

        bool wt(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const std::string &device_address,
            const std::uint8_t number_of_devices,
            const std::vector<std::uint32_t> &send_data
        )

        bool qt(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const std::string &device_address,
            const std::uint8_t number_of_devices,
            std::vector<std::uint32_t> &send_data
        );

        bool rr(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
        );

        bool rs(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
        );

        bool pc(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            std::vector<std::uint8_t> &recv_data
        );

        bool gw(
            const std::uint16_t branch_number,
            const std::uint16_t pc_number,
            const char telegram_wait,
            const bool signal_data
        );

      private:
        enum class CommandCode : std::uint16_t {
            BR = 0x4252,
            WR = 0x5752,
            QR = 0x5152,
            BW = 0x4257,
            WW = 0x5757,
            QW = 0x5157,
            BT = 0x4254,
            QT = 0x5154,
            RR = 0x5252,
            RS = 0x5352,
            PC = 0x5043,
            GW = 0x4757
        };

        enum class ControlAsciiCode : std::uint8_t {
            STX = 0x02,
            ETX = 0x03,
            EOT = 0x04,
            ENQ = 0x05,
            ACK = 0x06,
            LF  = 0x0A,
            CL  = 0x0C,
            CR  = 0x0D,
            NAK = 0x15,
        };


        bool append_checksum_flag_;
        bool append_crlf_flag_;
        std::string device_name_;

        Serial serial_;

        std::vector<std::uint8_t> internal_send_data_;
        std::vector<std::uint8_t> internal_recv_data_;

        char recv_checksum_buffer_[2] = { '\0 '};

        size_t read_until(std::uint8_t* buffer, const size_t max_read_size, const char delimiter);

        void finalize_send_data();

        bool recv_frame();
    };
}

#endif  // COMPUTER_LINK_HPP_