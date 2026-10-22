/**
 * @file computer_link.cpp
 * @author Sawada
 * @brief 
 * @version 0.1
 * @date 2026-10-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "protocol/computer_link.hpp"

#include <assert.h>
#include <time.h>

#include <vector>

namespace {
    /**
        @brief BRコマンドの基本フォーマット
        @note ENQ 1byte, 局番号 2byte, PC番号 2byte, 伝文待ち時間 1byte, @n
          先頭デバイスアドレス 5byte, デバイス点数 2byte
        @warning この形式が基本
    */
    inline constexpr char COMMAND_BR_BASIC_FMT[] = "%c%c%c%c%c%c%c%c%s%c%c";

    char convert_hex_to_ascii(const std::uint8_t hex_value)
    {
        assert(hex_value <= 15);
    
        constexpr char hex_digits[] = "0123456789ABCDEF";
    
        return hex_digits[hex_value];
    }
}

// 変更必須
ComputerLink::ComputerLink(const std::string& device_name, bool append_checksum_flag, bool append_crlf_flag)
    : serial_(device_name, Serial::baudrate_t::BAUDRATE_9600, 1000),
      append_checksum_flag_(append_checksum_flag),
      append_crlf_flag_(append_crlf_flag)
{
    send_data_.reserve(1024);
    send_data_.resize(1024);
}

bool ComputerLink::br(
    const std::uint16_t branch_number,
    const std::uint16_t pc_number,
    const char telegram_wait,
    const std::string &device_address,
    const std::uint8_t number_of_devices,
    std::vector<std::uint8_t> &recv_data
)
{
    // std::format() に置き換え -> COMMAND_BR_BASIC_FMTも変更必須
    snprintf(reinterpret_cast<char*>(send_data_.data()), send_data_.size(), COMMMAND_BR_BASIC_FMT,
        static_cast<char>(ControlAsciiCode::ENQ),
        static_cast<char>((static_cast<std::uint16_t>(CommandCode::BR) >> 8) & 0xFFU),
        static_cast<char>((static_cast<std::uint16_t>(CommandCode::BR)) & 0xFFU),
        convert_hex_to_ascii((branch_number >> 4) & 0x0FU),
        convert_hex_to_ascii(branch_number & 0x0FU),
        convert_hex_to_ascii((pc_number >> 4) & 0x0FU),
        convert_hex_to_ascii(pc_number & 0x0FU),
        telegram_wait,
        device_address.c_str(),
        convert_hex_to_ascii((number_of_devices >> 4) & 0x0FU),
        convert_hex_to_ascii(number_of_devices & 0x0FU)
    );
}

long ComputerLink::calculate_telegram_wait_to_ms(const std::uint8_t telegram_wait)
{
    /** @note telegram_wait はHEX形式で指定される */

    return static_cast<long>(telegram_wait) * 10;
}

bool ComputerLink::wait_ms(long ms)
{
    assert(ms >= 0);

    timespec req{};

    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000L;

    while (true) {
        int ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &req, &req);
        if (ret == 0) {
            return true;
        }
        if (ret != EINTR) {
            return false;
        }
    }
}


