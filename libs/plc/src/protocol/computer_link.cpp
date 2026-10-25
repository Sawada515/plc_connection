/**
 * @file computer_link.cpp
 * @author Sawada
 * @brief 
 * @note この層では例外を投げない。上位の層でassert()でチェックする
 * @version 0.1
 * @date 2026-10-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "protocol/computer_link.hpp"
#include "serial/serial.hpp"

#include <assert.h>
#include <time.h>

#include <vector>
#include <string>
#include <chrono>
#include <format>

namespace protocol {
    namespace {
        /**
         * @brief TRANSACTION_FORMAT トランザクションのフォーマット
         * @note ACK/NAK 1byte, 局番号 1byte, PC番号 1byte
         */
        inline constexpr std::string_view TRANSACTION_FORMAT = "{:c}{:02X}{:02X}";
    
        /**
            @brief BRコマンドの基本フォーマット
            @note ENQ 1byte, 局番号 1byte, PC番号 1byte, 伝文待ち時間 1byte, @n
              先頭デバイスアドレス 5byte, デバイス点数 1byte
            @warning この形式が基本
        */
        inline constexpr std::string_view COMMAND_BR_BASIC_FMT = "{:c}{:c}{:c}{:02X}{:02X}{:c}{}{:02X}";
    
        constexpr size_t ACK_RESPONSE_SIZE = 4;// 局番号 1byte, PC番号 1byte
        constexpr size_t NAK_RESPONSE_SIZE = 6;// 局番号 1byte, PC番号 1byte, エラーコード 2byte
    
        Serial::baudrate_t convert_baudrate_key_to_baudrate_t(unsigned int baudrate_key)
        {
            assert(baudrate_key <= 7);
    
            switch (baudrate_key) {
                case 0:
                    return Serial::baudrate_t::BAUDRATE_300;
                case 1:
                    return Serial::baudrate_t::BAUDRATE_600;
                case 2:
                    return Serial::baudrate_t::BAUDRATE_1200;
                case 3:
                    return Serial::baudrate_t::BAUDRATE_2400;
                case 4:
                    return Serial::baudrate_t::BAUDRATE_4800;
                case 5:
                    return Serial::baudrate_t::BAUDRATE_9600;
                case 6:
                    return Serial::baudrate_t::BAUDRATE_19200;
                case 7:
                    return Serial::baudrate_t::BAUDRATE_38400;
                default: 
                    /* 想定外 */
                    /** @note assert()でチェックするので入るはずがない */
                    break;
            }
        }
    
        char convert_hex_to_ascii(const std::uint8_t hex_value)
        {
            assert(hex_value <= 15);
        
            constexpr char hex_digits[] = "0123456789ABCDEF";
        
            return hex_digits[hex_value];
        }
    
        bool parse_recv_normal_data(std::vector<std::uint8_t> &recv_data)
        {
            char telegram_number[2] = { '\0' };
            char pc_number[2] = { '\0' };
            
        }
    
        bool parse_recv_abnormal_data(std::vector<std::uint8_t> &recv_data)
        {
            ;   // あと
        }
    
        long calculate_telegram_wait_to_ms(const std::uint8_t telegram_wait)
        {
            return static_cast<long>(telegram_wait) * 100;  // 100ms per unit
        }
    
        /**
         * @brief Calculate the checksum of the given data
         * 
         * @param[in] data Pointer to the data
         * @param[in] length Length of the data
         * @return std::uint16 The calculated checksum
         */
        std::uint8_t calculate_checksum(const std::uint8_t* data, size_t length)
        {
            std::uint32_t checksum = 0;
    
            for (size_t i = 0; i < length; ++i) {
                checksum += data[i];
            }
    
            return static_cast<std::uint8_t>(checksum & 0x000000FFU);
        }
    
        /**
         * @brief Wait for a specified number of milliseconds
         * 
         * @param[in] ms Number of milliseconds to wait
         * @return true if the wait was successful
         * @return false if an error occurred during the wait
         */
        bool wait_ms(long ms)
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
    };
    
    // 要確認
    ComputerLink::ComputerLink(
        const std::string& device_name,
        bool append_checksum_flag,
        bool append_crlf_flag,
        unsigned int baudrate_key,
        const int timeout_ms
    )
        : serial_(device_name, convert_baudrate_key_to_baudrate_t(baudrate_key), timeout_ms)
        , append_checksum_flag_(append_checksum_flag)
        , append_crlf_flag_(append_crlf_flag)
        , timeout_ms_(timeout_ms)
    {
        internal_send_data_.reserve(1024);
        internal_recv_data_.reserve(1024);

        transaction_send_data_.reserve(7);
    
        serial_.open();
    }
    
    ComputerLink::~ComputerLink()
    {
        serial_.close();
    }
    
    bool ComputerLink::br(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        std::vector<std::uint8_t> &recv_data
    )
    {
    
        internal_send_data_.clear();
    
        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_BR_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            static_cast<char>((static_cast<std::uint16_t>(CommandCode::BR) >> 8) & 0xFFU),
            static_cast<char>((static_cast<std::uint16_t>(CommandCode::BR)) & 0xFFU),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );
    
        finalize_send_data();
    
        serial_.write(internal_send_data_.data(), internal_send_data_.size());
    
        wait_ms(calculate_telegram_wait_to_ms(telegram_wait));
    
        internal_recv_data_.clear();
        internal_recv_data_.resize(1024);

        bool recv_result = recv_frame();
        bool parse_result = false;
    
        if (recv_result) {
            parse_result = parse_recv_normal_data(recv_data);
        }
        else {
            parse_result = parse_recv_abnormal_data(recv_data);
        }

        create_transaction_data(recv_result && parse_result, branch_number, pc_number);

        serial_.write(transaction_send_data_.data(), transaction_send_data_.size());

        return recv_result && parse_result;
    }

    bool ComputerLink::wr(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        std::vector<std::uint8_t> &recv_data
    )
    {
    
        internal_send_data_.clear();
    
        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_BR_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            static_cast<char>((static_cast<std::uint16_t>(CommandCode::WR) >> 8) & 0x00FFU),
            static_cast<char>((static_cast<std::uint16_t>(CommandCode::WR)) & 0x00FFU),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );
    
        finalize_send_data();
    
        serial_.write(internal_send_data_.data(), internal_send_data_.size());
    
        wait_ms(calculate_telegram_wait_to_ms(telegram_wait));
    
        internal_recv_data_.clear();
        internal_recv_data_.resize(1024);

        bool recv_result = recv_frame();
        bool parse_result = false;
    
        if (recv_result) {
            parse_result = parse_recv_normal_data(recv_data);
        }
        else {
            parse_result = parse_recv_abnormal_data(recv_data);
        }

        create_transaction_data(recv_result && parse_result, branch_number, pc_number);

        serial_.write(transaction_send_data_.data(), transaction_send_data_.size());

        return recv_result && parse_result;
    }

    bool ComputerLink::qr(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        std::vector<std::uint8_t> &recv_data
    ) 
    {
    
        internal_send_data_.clear();
    
        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_BR_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            static_cast<char>((static_cast<std::uint16_t>(CommandCode::QR) >> 8) & 0x00FFU),
            static_cast<char>((static_cast<std::uint16_t>(CommandCode::QR)) & 0x00FFU),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );
    
        finalize_send_data();
    
        serial_.write(internal_send_data_.data(), internal_send_data_.size());
    
        wait_ms(calculate_telegram_wait_to_ms(telegram_wait));
    
        internal_recv_data_.clear();
        internal_recv_data_.resize(1024);

        bool recv_result = recv_frame();
        bool parse_result = false;
    
        if (recv_result) {
            parse_result = parse_recv_normal_data(recv_data);
        }
        else {
            parse_result = parse_recv_abnormal_data(recv_data);
        }

        create_transaction_data(recv_result && parse_result, branch_number, pc_number);

        serial_.write(transaction_send_data_.data(), transaction_send_data_.size());

        return recv_result && parse_result;
    }
    
    
    // Private member 
    void ComputerLink::finalize_send_data()
    {
        if (append_checksum_flag_) {
            std::uint8_t checksum = calculate_checksum(internal_send_data_.data() + 1, internal_send_data_.size());
    
            internal_send_data_.push_back(convert_hex_to_ascii((checksum >> 4) & 0x0FU));
            internal_send_data_.push_back(convert_hex_to_ascii((checksum) & 0x0FU));
        }
    
        if (append_crlf_flag_) {
            internal_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::CR));
            internal_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::LF));
        }
    }
    
    size_t ComputerLink::read_until(std::uint8_t* buffer, const size_t max_read_size, const char delimiter)
    {
        size_t total_bytes_read = 0;
    
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms_);
    
        while (total_bytes_read < max_read_size) {
            if (std::chrono::steady_clock::now() >= deadline) {
                return false; // タイムアウト
            }
    
            ssize_t bytes_read = serial_.read(buffer + total_bytes_read, 1);
    
            if (bytes_read < 0) {
                return total_bytes_read;
            }
            else if (bytes_read == 0) {
                continue;
            }
    
            total_bytes_read += static_cast<size_t>(bytes_read);
    
            if (buffer[total_bytes_read - 1] == static_cast<std::uint8_t>(delimiter)) {
                break;  // デリミタに到達
            }
        }
    
        return total_bytes_read;
    }
    
    // 戻り値とベクターの中身を必ず確認
    // trueの場合は正常データ
    // falseの場合は異常データの可能性とタイムアウトの可能性があるのでvectorの中身必ずを確認すること
    bool ComputerLink::recv_frame()
    {
        // 1バイト読み取り用バッファ
        std::uint8_t first_char = 0;
        std::uint8_t discard_buffer[2];
    
        internal_recv_data_.clear();
    
        recv_checksum_buffer_[0] = '\0';
        recv_checksum_buffer_[1] = '\0';
    
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms_);
    
        // 先頭文字 (STX / ACK / NAK) の受信待機
        while (true) {
            if (std::chrono::steady_clock::now() >= deadline) {
                return false; // タイムアウト
            }
    
            ssize_t bytes_read = serial_.read(&first_char, 1);
            if (bytes_read > 0) {
                break;
            }
            if (bytes_read < 0) {
                return false;
            }
    
            wait_ms(2);
        }
    
        if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::STX)) {
            internal_recv_data_.resize(internal_recv_data_.capacity());
    
            ssize_t ret = read_until(internal_recv_data_.data(), internal_recv_data_.size(), static_cast<char>(ControlAsciiCode::ETX));
            if (ret <= 0) {
                internal_recv_data_.clear();
                return false;
            }
    
            internal_recv_data_.resize(static_cast<size_t>(ret));
    
            if (append_checksum_flag_) {
                char checksum_str[2];
                ssize_t checksum_read = serial_.read(reinterpret_cast<std::uint8_t*>(checksum_str), 2);
                if (checksum_read != 2) {
                    internal_recv_data_.clear();
    
                    return false;
                }
    
                recv_checksum_buffer_[0] = checksum_str[0];
                recv_checksum_buffer_[1] = checksum_str[1];
            }
    
            if (append_crlf_flag_) {
                serial_.read(discard_buffer, 2);
            }
    
            return true;
        }
        else if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::ACK)) {
            internal_recv_data_.resize(ACK_RESPONSE_SIZE);
    
            ssize_t ret = serial_.read(internal_recv_data_.data(), ACK_RESPONSE_SIZE);
            if (ret != ACK_RESPONSE_SIZE) {
                internal_recv_data_.clear();
                return false;
            }
    
            if (append_crlf_flag_) {
                read_until(discard_buffer, 2, static_cast<char>(ControlAsciiCode::LF));
            }
    
            return true;
        }
        else if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::NAK)) {
            internal_recv_data_.resize(NAK_RESPONSE_SIZE);
    
            ssize_t ret = serial_.read(internal_recv_data_.data(), NAK_RESPONSE_SIZE);
            if (ret != NAK_RESPONSE_SIZE) {
                internal_recv_data_.clear();
                return false;
            }
    
            if (append_crlf_flag_) {
                serial_.read(discard_buffer, 2);
            }
    
            return false;
        }
    
        return false;
    }

    void ComputerLink::create_transaction_data(bool is_error, std::uint8_t branch_number, std::uint8_t pc_number)
    {
        if (!is_error) {
            std::format_to(
                std::back_inserter(internal_send_data_),
                TRANSACTION_FORMAT,
                static_cast<char>(ControlAsciiCode::ACK),
                branch_number,
                pc_number 
            );
        }
        else {
            std::format_to(
                std::back_inserter(internal_send_data_),
                TRANSACTION_FORMAT,
                static_cast<char>(ControlAsciiCode::NAK),
                branch_number,
                pc_number 
            );
        }

        if (append_crlf_flag_) {
            internal_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::CR));
            internal_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::LF));
        }
    }
}
