/**
 * @file communication.hpp
 * @author Sawada
 * @brief PLC Communication header file
 * @version 0.1
 * @date 2026-10-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <cstdint>
#include <vector>
#include <string>

class Communication {
  public:
    /**
    * @brief Common arguments for communication
    *
    */
    struct CommonArgs {
        unsigned char branch_number;    /** branch_number 局番号*/
        unsigned char pc_number;        /** pc_number PC番号*/
        unsigned char telegram_wait;    /** telegram_wait 伝文ウェイト */
    };

    struct TestWriteData {
        std::string device_address;     /** device_address デバイスアドレス */
        std::uint32_t set_data;         /** set_data 設定データ */
    };

    enum class SerialBaudRate : unsigned int {
        BAUDRATE_300    = 0,
        BAUDRATE_600    = 1,
        BAUDRATE_1200   = 2,
        BAUDRATE_2400   = 3,
        BAUDRATE_4800   = 4,
        BAUDRATE_9600   = 5,
        BAUDRATE_19200  = 6,
        BAUDRATE_38400  = 7
    };

    /**
     * @brief Construct a new Communication object
     */
    Communication(
        const std::string& device_name,
        const bool append_checksum_flag,
        const bool append_crlf_flag,
        const SerialBaudRate baudrate // <= assertによるチェックを行う
    );

    /**
     * @brief Destroy the Communication object
     */
    ~Communication();

    bool
    read_bit_device(
        const CommonArgs common_args,
        const std::string& device_address,
        const unsigned char number_of_devices,
        std::vector<std::uint8_t> &recv_data
    );

    bool
    read_word_device(
        const CommonArgs common_args,
        const std::string& device_address,
        const unsigned char number_of_device,
        std::vector<std::uint16_t> &recv_data
    );

    bool
    write_bit_device(
        const CommonArgs common_args,
        const std::string& device_address,
        const unsigned char number_of_devices,
        const std::vector<std::uint8_t> &send_data
    );

    bool
    write_word_device(
        const CommonArgs common_args,
        const std::string& device_address,
        const unsigned char number_of_devices,
        const std::vector<std::uint16_t> &send_data
    );

    bool
    test_bit_device(
        const CommonArgs common_args,
        const std::vector<TestWriteData> send_data
    );

    bool
    test_word_device(
        const CommonArgs common_args,
        const std::vector<TestWriteData> send_data
    );

    bool
    remote_run(
        const CommonArgs common_args
    );

    bool
    remote_stop(
        const CommonArgs common_args
    );

    void
    write_global_signal(
        const CommonArgs common_args,
        const bool signal_data
    );
    
  private:
    std::string device_name_;
    bool append_checksum_flag_;
    bool append_crlf_flag_;
};