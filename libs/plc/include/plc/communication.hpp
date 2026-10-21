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

    struct DeviceAddress {
        char device_preffix;            /** device_prefix デバイス接頭文字 */
        unsigned int device_number;     /** device_number デバイス番号 */
    };

    struct TestWriteData {
        DeviceAddress device_address;   /** device_address デバイスアドレス */
        std::uint32_t set_data;         /** set_data 設定データ */
    };

    enum class ProtocolType {
        ComputerLinkTypeNoCRLF,         /** ComputerLinkTypeNoCRLF データの末尾にCRとLFを付加しないプロトコル */
        ComputerLinkTypeWithCRLF,       /** ComputerLinkTypeWithCRLF  データの末尾にCRとLFを付加するプロトコル */
        ModbusRTU                       /** ModbusRTU 未実装 */
    };

    /**
     * @brief Construct a new Communication object
     */
    Communication(
        const ProtocolType protocol_type,
        const std::string& device_name,
        const bool append_checksum_flag
    );

    /**
     * @brief Destroy the Communication object
     */
    ~Communication();

    bool
    read_bit_device(
        const CommonArgs common_args,
        const DeviceAddress device_address,
        const unsigned char number_of_devices,
        std::vector<std::uint8_t> &recv_data
    );

    bool
    read_word_device(
        const CommonArgs common_args,
        const DeviceAddress device_address,
        const unsigned char number_of_device,
        std::vector<std::uint16_t> &recv_data
    );

    bool
    write_bit_device(
        const CommonArgs common_args,
        const DeviceAddress device_address,
        const unsigned char number_of_devices,
        const std::vector<std::uint8_t> &send_data
    );

    bool
    write_word_device(
        const CommonArgs common_args,
        const DeviceAddress device_address,
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
    ProtocolType protocol_type_;
    std::string device_name_;
    bool append_checksum_flag_;

    void* protocol_handler_;
};