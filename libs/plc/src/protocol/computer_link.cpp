/**
 * @file computer_link.cpp
 * @author Sawada
 * @brief
 * @note この層ではシリアルオープン以外の例外を投げない。上位の層でassert()でチェックする
 * @version 0.1
 * @date 2026-10-21
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <protocol/computer_link.hpp>
#include <serial/serial.hpp>

#include <assert.h>
#include <time.h>

#include <vector>
#include <string>
#include <chrono>
#include <format>
#include <stdexcept>

#define DEBUG
/* DEBUG時は7.5秒の待ちが必ず入る */

#ifdef DEBUG
#include <iostream>

void debug_print(const std::vector<std::uint8_t> &data)
{
    for (int i = 0; i < data.size(); ++i) {
        printf("%02X: %c\n", data[i], data[i]);
    }
}
#else
void debug_print(const std::vector<std::uint8_t> &data __attribute__((unused)))
{
    ;
}

#endif // DEBUG

namespace protocol
{
    namespace
    {
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
        inline constexpr std::string_view COMMAND_BR_BASIC_FMT = "{:c}{:02X}{:02X}BR{:c}{:.5}{:02X}";
        inline constexpr std::string_view COMMAND_WR_BASIC_FMT = "{:c}{:02X}{:02X}WR{:c}{:.5}{:02X}";
        inline constexpr std::string_view COMMAND_QR_BASIC_FMT = "{:c}{:02X}{:02X}QR{:c}{:.7}{:02X}";

        inline constexpr std::string_view COMMAND_BW_BASIC_FMT = "{:c}{:02X}{:02X}BW{:c}{:.5}{:02X}";
        inline constexpr std::string_view COMMAND_WW_BASIC_FMT = "{:c}{:02X}{:02X}WW{:c}{:.5}{:02X}";
        inline constexpr std::string_view COMMAND_QW_BASIC_FMT = "{:c}{:02X}{:02X}QW{:c}{:.7}{:02X}";

        inline constexpr std::string_view COMMAND_BT_BASIC_FMT = "{:c}{:02X}{:02X}BT{:c}{:02X}";
        inline constexpr std::string_view COMMAND_BT_PYALOAD_FMT = "{:.5}{:c}";
        inline constexpr std::string_view COMMAND_WT_BASIC_FMT = "{:c}{:02X}{:02X}WT{:c}{:02X}";
        inline constexpr std::string_view COMMAND_WT_PYALOAD_FMT = "{:.5}{:c}";
        inline constexpr std::string_view COMMAND_QT_BASIC_FMT = "{:c}{:02X}{:02X}QT{:c}{:02X}";
        inline constexpr std::string_view COMMAND_QT_PYALOAD_FMT = "{:.7}{:c}";

        inline constexpr std::string_view COMMAND_RR_BASIC_FMT = "{:c}{:02X}{:02X}RR{:c}";
        inline constexpr std::string_view COMMAND_RS_BASIC_FMT = "{:c}{:02X}{:02X}RS{:c}";

        inline constexpr std::string_view COMMAND_PC_BASIC_FMT = "{:c}{:02X}{:02X}PC{:c}";

        inline constexpr std::string_view COMMAND_GW_BASIC_FMT = "{:c}{:02X}{:02X}GW{:c}{:c}";

        constexpr size_t ACK_RESPONSE_SIZE = 4; // 局番号 1byte, PC番号 1byte
        constexpr size_t NAK_RESPONSE_SIZE = 6; // 局番号 1byte, PC番号 1byte, エラーコード 2byte

        Serial::baudrate_t convert_baudrate_key_to_baudrate_t(unsigned int baudrate_key)
        {
            assert(baudrate_key <= 7);

            switch (baudrate_key)
            {
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
                    assert(false);
    
                    break;
            }
        }

        char convert_hex_to_ascii(const std::uint8_t hex_value)
        {
            assert(hex_value <= 15);

            constexpr char hex_digits[] = "0123456789ABCDEF";

            return hex_digits[hex_value];
        }

        std::uint8_t convert_ascii_to_hex(const char ascii_char)
        {
            assert((ascii_char >= '0' && ascii_char <= '9') || (ascii_char >= 'A' && ascii_char <= 'F') || (ascii_char >= 'a' && ascii_char <= 'f'));

            if (ascii_char >= '0' && ascii_char <= '9')
            {
                return static_cast<std::uint8_t>(ascii_char - '0');
            }
            else if (ascii_char >= 'A' && ascii_char <= 'F')
            {
                return static_cast<std::uint8_t>(ascii_char - 'A' + 10);
            }
            else /* a - z */
            {
                return static_cast<std::uint8_t>(ascii_char - 'a' + 10);
            }
        }

        long calculate_telegram_wait_to_ms(const std::uint8_t telegram_wait)
        {
            return static_cast<long>(convert_ascii_to_hex(telegram_wait) * 10);
        }

        /**
         * @brief Calculate the checksum of the given data
         *
         * @param[in] data Pointer to the data
         * @param[in] length Length of the data
         * @return std::uint16 The calculated checksum
         */
        std::uint8_t calculate_checksum(const std::uint8_t *data, size_t length)
        {
            std::uint32_t checksum = 0;

            for (size_t i = 0; i < length; ++i)
            {
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

            while (true)
            {
                int ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &req, &req);
                if (ret == 0)
                {
                    return true;
                }
                if (ret != EINTR)
                {
                    return false;
                }
            }
        }
    };

    // 要確認
    ComputerLink::ComputerLink(
        const std::string &device_name,
        bool append_checksum_flag,
        bool append_crlf_flag,
        unsigned int baudrate_key,
        const int timeout_ms)
        : serial_(device_name, convert_baudrate_key_to_baudrate_t(baudrate_key), timeout_ms), append_checksum_flag_(append_checksum_flag), append_crlf_flag_(append_crlf_flag), timeout_ms_(timeout_ms)
    {
        internal_send_data_.reserve(MAX_BUFFER_SIZE);
        internal_recv_data_.reserve(MAX_BUFFER_SIZE);

        transaction_send_data_.reserve(7);

        if (!serial_.open()) {
            throw std::runtime_error("Failed to open serial port: " + device_name);
        }
    }

    ComputerLink::~ComputerLink()
    {
        serial_.close();
    }

    ComputerLink::CommandStatus ComputerLink::br(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        std::vector<std::uint8_t> &recv_data)
    {
        internal_send_data_.clear();

        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_BR_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );

        finalize_send_data();

        debug_print(internal_send_data_);

        serial_.write(internal_send_data_.data(), internal_send_data_.size());

        /** @note number_of_devices * 4 <= BRコマンドはbit数で指定するためそのまま */
        return execute_read_command_transaction(branch_number, pc_number, telegram_wait, number_of_devices, recv_data);
    }

    ComputerLink::CommandStatus ComputerLink::wr(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        std::vector<std::uint8_t> &recv_data)
    {
        internal_send_data_.clear();

        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_WR_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );

        finalize_send_data();

        debug_print(internal_send_data_);

        serial_.write(internal_send_data_.data(), internal_send_data_.size());

        /** @note number_of_devices * 4 <= これはPLCに1byteのデータを要求すると4キャラクタ分のデータで返されるため */
        return execute_read_command_transaction(branch_number, pc_number, telegram_wait, number_of_devices * 4, recv_data);
    }

    ComputerLink::CommandStatus ComputerLink::qr(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        std::vector<std::uint8_t> &recv_data)
    {
        internal_send_data_.clear();

        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_QR_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );

        finalize_send_data();

        debug_print(internal_send_data_);

        serial_.write(internal_send_data_.data(), internal_send_data_.size());

        /** @note number_of_devices * 4 <= これはPLCに1byteのデータを要求すると4キャラクタ分のデータで返されるため */
        return execute_read_command_transaction(branch_number, pc_number, telegram_wait, number_of_devices * 4, recv_data);
    }

    ComputerLink::CommandStatus ComputerLink::bw(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        const std::vector<std::uint8_t> &send_data
    )
    {
        internal_send_data_.clear();

        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_BW_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );

        internal_send_data_.insert(internal_send_data_.end(), send_data.begin(), send_data.end());

        finalize_send_data();

        debug_print(internal_send_data_);

        serial_.write(internal_send_data_.data(), internal_send_data_.size());

        return execute_write_command_transaction(branch_number, pc_number, telegram_wait);
    }

    ComputerLink::CommandStatus ComputerLink::ww(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        const std::vector<std::uint8_t> &send_data
    )
    {
        internal_send_data_.clear();

        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_WW_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );

        internal_send_data_.insert(internal_send_data_.end(), send_data.begin(), send_data.end());

        finalize_send_data();

        debug_print(internal_send_data_);

        serial_.write(internal_send_data_.data(), internal_send_data_.size());

        return execute_write_command_transaction(branch_number, pc_number, telegram_wait);
    }

    ComputerLink::CommandStatus ComputerLink::qw(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait,
        const std::string &device_address,
        const std::uint8_t number_of_devices,
        const std::vector<std::uint8_t> &send_data
    )
    {
        internal_send_data_.clear();

        std::format_to(
            std::back_inserter(internal_send_data_),
            COMMAND_QW_BASIC_FMT,
            static_cast<char>(ControlAsciiCode::ENQ),
            branch_number,
            pc_number,
            telegram_wait,
            device_address,
            number_of_devices
        );

        internal_send_data_.insert(internal_send_data_.end(), send_data.begin(), send_data.end());

        finalize_send_data();

        debug_print(internal_send_data_);

        serial_.write(internal_send_data_.data(), internal_send_data_.size());

        return execute_write_command_transaction(branch_number, pc_number, telegram_wait);
    }

    /**
     * @brief コマンドの実行結果を文字列に変換する
     * @param status コマンドの実行結果
     * @return 変換された文字列
     */
    const char* ComputerLink::convert_command_status_to_string(CommandStatus status)
    {
        switch (status) {
            case CommandStatus::Success:
                return "Success";
            case CommandStatus::Timeout:
                return "Timeout";
            case CommandStatus::UnknownError:
                return "UnknownError";
            case CommandStatus::InvalidHeaderError:
                return "InvalidHeaderError";
            case CommandStatus::RxChecksumError:
                return "RxChecksumError";
            case CommandStatus::RxLengthError:
                return "RxLengthError";
            case CommandStatus::RxBranchNumberMismatchError:
                return "RxBranchNumberMismatchError";
            case CommandStatus::RxPcNumberMismatchError:
                return "RxPcNumberMismatchError";
            case CommandStatus::HasError:
                return "HasError";  /**@note これは内部用なので外に出ないはず */
            case CommandStatus::ChecksumError:
                return "ChecksumError";
            case CommandStatus::ProtocolError:
                return "ProtocolError";
            case CommandStatus::CharacterPartError:
                return "CharacterPartError";
            case CommandStatus::CharacterError:
                return "CharacterError";
            case CommandStatus::PcNumberError:
                return "PcNumberError";
            case CommandStatus::RemoteError:
                return "RemoteError";
            default:
                return "Unknown Status";
        }
    }

    // Private member
    /**
     * @brief トランザクションを実行する
     * @param branch_number ブランチ番号
     * @param pc_number PC番号
     * @param telegram_wait トランザクションの待機時間
     * @param number_of_devices デバイス点数
     * @param recv_data 受信データ
     * @return 実行結果
     * @warning number_of_devicesは必ずbit数で指定する。BRコマンドの仕様に合わせている
     */
    ComputerLink::CommandStatus ComputerLink::execute_read_command_transaction(
            const std::uint8_t branch_number,
            const std::uint8_t pc_number,
            const char telegram_wait,
            const std::uint8_t number_of_devices,
            std::vector<std::uint8_t> &recv_data
    )
    {
        wait_ms(calculate_telegram_wait_to_ms(telegram_wait));

#ifdef DEBUG
        for (int i = 0; i < 50; ++i) {
            wait_ms(calculate_telegram_wait_to_ms('F'));
        }
#endif

        internal_recv_data_.clear();

        CommandStatus recv_result = recv_frame();
        CommandStatus parse_result{};

        if (recv_result == CommandStatus::Success) {
            parse_result = parse_recv_normal_data(recv_data, branch_number, pc_number, number_of_devices, true);
        }
        else if (recv_result == CommandStatus::HasError) {
            parse_result = parse_recv_abnormal_data();
        }

        if (recv_result == CommandStatus::Success) {
            bool has_error = (parse_result != CommandStatus::Success);

            transaction_send_data_.clear();

            create_transaction_data(has_error, branch_number, pc_number);

            serial_.write(transaction_send_data_.data(), transaction_send_data_.size());

            return parse_result;
        }
        else if (recv_result == CommandStatus::HasError) {
            /* PLCが返したエラー */

            return parse_result;
        }
        else {
            return recv_result;
        }
    }

    ComputerLink::CommandStatus ComputerLink::execute_write_command_transaction(
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const char telegram_wait
    )
    {
        wait_ms(calculate_telegram_wait_to_ms(telegram_wait));

        internal_recv_data_.clear();

        CommandStatus recv_result = recv_frame();
        CommandStatus parse_result{};

        std::vector<std::uint8_t> recv_data __attribute__((unused)); // 受信データはwrite系コマンドでは使用しない

        if (recv_result == CommandStatus::Success) {
            // write系コマンドでは、受信データの解析は行わないため、number_of_devicesは0として呼び出す

            parse_result = parse_recv_normal_data(recv_data, branch_number, pc_number, 0, false);
        }
        else if (recv_result == CommandStatus::HasError) {
            parse_result = parse_recv_abnormal_data();
        }

        if (recv_result == CommandStatus::Success) {
            return parse_result;
        }
        else if (recv_result == CommandStatus::HasError) {
            /* PLCが返したエラー */

            return parse_result;
        }
        else {
            return recv_result;
        }
    }

    void ComputerLink::finalize_send_data()
    {
        if (append_checksum_flag_) {
            std::uint8_t checksum = calculate_checksum(internal_send_data_.data() + 1, internal_send_data_.size() - 1);

            internal_send_data_.push_back(convert_hex_to_ascii((checksum >> 4) & 0x0FU));
            internal_send_data_.push_back(convert_hex_to_ascii((checksum) & 0x0FU));
        }

        if (append_crlf_flag_) {
            internal_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::CR));
            internal_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::LF));
        }
    }

    ssize_t ComputerLink::read_until(std::uint8_t *buffer, const size_t max_read_size, const char delimiter)
    {
        size_t total_bytes_read = 0;

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms_);

        while (total_bytes_read < max_read_size) {
            if (std::chrono::steady_clock::now() >= deadline) {
                return -1; // タイムアウト
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
                break; // デリミタに到達
            }
        }

        return total_bytes_read;
    }

    ComputerLink::CommandStatus ComputerLink::recv_frame()
    {
        std::uint8_t first_char = 0; /** @brief 正常データの先頭文字読み取り用 */
        std::uint8_t discard_buffer[2];

        internal_recv_data_.clear();

        recv_checksum_buffer_[0] = '\0';
        recv_checksum_buffer_[1] = '\0';

        unsigned char try_count = 0;
        constexpr unsigned char MAX_TRY_COUNT = 15;

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms_);

        // 先頭文字 (STX / ACK / NAK) の受信待機
        while (true) {
            if (std::chrono::steady_clock::now() >= deadline) {
                return CommandStatus::Timeout;
            }

            ssize_t bytes_read = serial_.read(&first_char, 1);
            if (bytes_read > 0) {
                if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::STX) ||
                    first_char == static_cast<std::uint8_t>(ControlAsciiCode::ACK) ||
                    first_char == static_cast<std::uint8_t>(ControlAsciiCode::NAK)) {

                    break; // 正常データの先頭文字を受信
                }

                try_count += 1;
                if (try_count >= MAX_TRY_COUNT) {
                    return CommandStatus::InvalidHeaderError; 
                }
            }
            if (bytes_read < 0) {
                return CommandStatus::UnknownError;
            }
        }

        if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::STX)) {
            internal_recv_data_.resize(MAX_BUFFER_SIZE);

            ssize_t ret = read_until(internal_recv_data_.data(), internal_recv_data_.size(), static_cast<char>(ControlAsciiCode::ETX));
            if (ret == -1) {
                internal_recv_data_.clear();

                return CommandStatus::Timeout;
            }
            else if (ret == 0) {
                internal_recv_data_.clear();

                return CommandStatus::UnknownError;
            }

            internal_recv_data_.resize(static_cast<size_t>(ret));

            if (append_checksum_flag_) {
                char checksum_str[2];
                ssize_t checksum_read = serial_.read(reinterpret_cast<std::uint8_t *>(checksum_str), 2);
                if (checksum_read != 2) {
                    internal_recv_data_.clear();

                    return CommandStatus::UnknownError;
                }

                recv_checksum_buffer_[0] = checksum_str[0];
                recv_checksum_buffer_[1] = checksum_str[1];
            }

            if (append_crlf_flag_) {
                read_until(discard_buffer, 2, static_cast<char>(ControlAsciiCode::LF));
            }

            return CommandStatus::Success;
        }
        else if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::ACK)) {
            internal_recv_data_.resize(ACK_RESPONSE_SIZE);

            ssize_t ret = serial_.read(internal_recv_data_.data(), ACK_RESPONSE_SIZE);
            if (ret != ACK_RESPONSE_SIZE) {
                internal_recv_data_.clear();

                return CommandStatus::UnknownError;
            }

            if (append_crlf_flag_) {
                read_until(discard_buffer, 2, static_cast<char>(ControlAsciiCode::LF));
            }

            return CommandStatus::Success;
        }
        else if (first_char == static_cast<std::uint8_t>(ControlAsciiCode::NAK)) {
            internal_recv_data_.resize(NAK_RESPONSE_SIZE);

            ssize_t ret = serial_.read(internal_recv_data_.data(), NAK_RESPONSE_SIZE);
            if (ret != NAK_RESPONSE_SIZE) {
                internal_recv_data_.clear();

                return CommandStatus::UnknownError;
            }

            if (append_crlf_flag_) {
                read_until(discard_buffer, 2, static_cast<char>(ControlAsciiCode::LF));
            }

            return CommandStatus::HasError;
        }

        return CommandStatus::UnknownError;
    }

    void ComputerLink::create_transaction_data(bool has_error, std::uint8_t branch_number, std::uint8_t pc_number)
    {
        if (has_error == false) {
            std::format_to(
                std::back_inserter(transaction_send_data_),
                TRANSACTION_FORMAT,
                static_cast<char>(ControlAsciiCode::ACK),
                branch_number,
                pc_number);
        }
        else {
            std::format_to(
                std::back_inserter(transaction_send_data_),
                TRANSACTION_FORMAT,
                static_cast<char>(ControlAsciiCode::NAK),
                branch_number,
                pc_number);
        }

        if (append_crlf_flag_) {
            transaction_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::CR));
            transaction_send_data_.push_back(static_cast<std::uint8_t>(ControlAsciiCode::LF));
        }
    }

    /**
     * @brief 受信した通常データを解析する
     * @param recv_data 受信したデータ
     * @param number_of_devices デバイス点数
     * @return 解析結果
     * @warning number_of_devicesは必ずbit数で指定する。BRコマンドの仕様に合わせている
     */
    ComputerLink::CommandStatus ComputerLink::parse_recv_normal_data(
        std::vector<std::uint8_t> &recv_data,
        const std::uint8_t branch_number,
        const std::uint8_t pc_number,
        const std::uint8_t number_of_devices,
        const bool call_from_execute_read_command_transaction
    )
    {
        char recv_branch_number[2] = {'\0'};
        char recv_pc_number[2] = {'\0'};

        recv_branch_number[0] = static_cast<char>(internal_recv_data_[0]);
        recv_branch_number[1] = static_cast<char>(internal_recv_data_[1]);
        recv_pc_number[0] = static_cast<char>(internal_recv_data_[2]);
        recv_pc_number[1] = static_cast<char>(internal_recv_data_[3]);

        std::uint8_t recv_branch_number_value = (convert_ascii_to_hex(recv_branch_number[0]) << 4) | convert_ascii_to_hex(recv_branch_number[1]);
        std::uint8_t recv_pc_number_value = (convert_ascii_to_hex(recv_pc_number[0]) << 4) | convert_ascii_to_hex(recv_pc_number[1]);

        if (recv_branch_number_value != branch_number) {
            return CommandStatus::RxBranchNumberMismatchError;
        }
        if (recv_pc_number_value != pc_number) {
            return CommandStatus::RxPcNumberMismatchError;
        }

        /*
         * write系コマンドは局番号とPC番号のみ送られてくる
         */
        if (!call_from_execute_read_command_transaction) {
            return CommandStatus::Success;
        }

        recv_data.clear();

        const size_t FRAME_HEADER_SIZE = 4;
        size_t data_count = FRAME_HEADER_SIZE;

        while (data_count < internal_recv_data_.size() && static_cast<char>(internal_recv_data_[data_count]) != static_cast<char>(ControlAsciiCode::ETX)) {
            recv_data.push_back(internal_recv_data_[data_count]);

            data_count += 1;
        }

        if (recv_data.size() != static_cast<std::size_t>(number_of_devices)) {
            return CommandStatus::RxLengthError;
        }

        if (append_checksum_flag_) {
            std::uint8_t checksum = calculate_checksum(internal_recv_data_.data(), data_count + 1);
            std::uint8_t recved_checksum = (convert_ascii_to_hex(recv_checksum_buffer_[0]) << 4) | convert_ascii_to_hex(recv_checksum_buffer_[1]);

            if (checksum != recved_checksum) {
                return CommandStatus::RxChecksumError;
            }
        }

        return CommandStatus::Success;
    }

    ComputerLink::CommandStatus ComputerLink::parse_recv_abnormal_data()
    {
        char telegram_number[2] = {'\0'};
        char pc_number[2] = {'\0'};

        if (internal_recv_data_.size() < 6) {
            return CommandStatus::Timeout; /** @note タイムアウトの可能性 */
        }

        telegram_number[0] = static_cast<char>(internal_recv_data_[0]);
        telegram_number[1] = static_cast<char>(internal_recv_data_[1]);
        pc_number[0] = static_cast<char>(internal_recv_data_[2]);
        pc_number[1] = static_cast<char>(internal_recv_data_[3]);

        unsigned char error_code = (convert_ascii_to_hex((static_cast<char>(internal_recv_data_[4]))) << 4) |
            convert_ascii_to_hex(static_cast<char>(internal_recv_data_[5]));

        return convert_error_code_to_command_status(error_code);
    }

    ComputerLink::CommandStatus ComputerLink::convert_error_code_to_command_status(std::uint8_t error_code)
    {
        switch (error_code) {
            case 0x02:
                return CommandStatus::ChecksumError;
            case 0x03:
                return CommandStatus::ProtocolError;
            case 0x06:
                return CommandStatus::CharacterPartError;
            case 0x07:
                return CommandStatus::CharacterError;
            case 0x0A:
                return CommandStatus::PcNumberError;
            case 0x10:
                return CommandStatus::PcNumberError;
            case 0x18:
                return CommandStatus::RemoteError;
            default:
                return CommandStatus::UnknownError;
        }
    }
}
