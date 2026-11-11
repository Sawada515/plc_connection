/**
 * @file computer_link_test.cpp
 * @author sawada
 * @brief  computer link class test 
 * @date 2026-11-06
 * @note 計算機リンクプロトコルの動作確認
 */

#include <plc/protocol/computer_link.hpp>

#include <errno.h>

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <chrono>

constexpr size_t BUFFER_SIZE = 256;

constexpr std::uint8_t BRANCH_NUMBER = 0x00;
constexpr std::uint8_t PC_NUMBER = 0xFF;
constexpr char TELEGRAM_WAIT = '1';

int main()
{
    protocol::ComputerLink::CommandStatus ret;

    std::vector<std::uint8_t> recv_data;
    recv_data.reserve(BUFFER_SIZE);

    try {
        // チェックサムなし　CRLFなし   OK
        protocol::ComputerLink computer_link("/dev/ttyUSB0", false, false, 5, 2000);
        // チェックサムあり　CRLFなし   OK
        //protocol::ComputerLink computer_link("/dev/ttyUSB0", true, false, 5, 10);
        // チェックサムなし　CRLFあり   OK
        //protocol::ComputerLink computer_link("/dev/ttyUSB0", false, true, 5, 10);
        // チェックサムあり CRLFあり
        //protocol::ComputerLink computer_link("/dev/ttyUSB0", true, true, 5, 10);

        ret = computer_link.br(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "M0252",
            1,
            recv_data
        );

        std::cout << "BR command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        if (ret == protocol::ComputerLink::CommandStatus::Success) {
            std::cout << "Received data: ";
            for (const auto& byte : recv_data) {
                std::cout << std::hex << static_cast<int>(byte) << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Command failed with status: " << static_cast<int>(ret) << std::endl;
        }

        ret = computer_link.wr(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "D8431",
            1,
            recv_data
        );

        std::cout << "WR command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        if (ret == protocol::ComputerLink::CommandStatus::Success) {
            std::cout << "Received data: ";
            for (const auto& byte : recv_data) {
                std::cout << std::hex << static_cast<int>(byte) << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Command failed with status: " << static_cast<int>(ret) << std::endl;
        }

        ret = computer_link.qr(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "D008431",
            1,
            recv_data
        );

        std::cout << "QR command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        if (ret == protocol::ComputerLink::CommandStatus::Success) {
            std::cout << "Received data: ";
            for (const auto& byte : recv_data) {
                std::cout << std::hex << static_cast<int>(byte) << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Command failed with status: " << static_cast<int>(ret) << std::endl;
        }

        ret = computer_link.pc(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            recv_data
        );

        std::cout << "PC command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        if (ret == protocol::ComputerLink::CommandStatus::Success) {
            std::cout << "Received data: ";
            for (const auto& byte : recv_data) {
                std::cout << std::hex << static_cast<int>(byte) << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Command failed with status: " << static_cast<int>(ret) << std::endl;
        }

        // write系コマンドのテスト

        std::vector<std::uint8_t> send_data;
        send_data.reserve(BUFFER_SIZE);

        send_data.push_back(0x30);
        send_data.push_back(0x31);
        send_data.push_back(0x31);
        send_data.push_back(0x30);
        send_data.push_back(0x31);

        ret = computer_link.bw(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "M0903",
            5,
            send_data
        );

        std::cout << "BW command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        send_data.clear();

        send_data.push_back(0x32);
        send_data.push_back(0x33);
        send_data.push_back(0x34);
        send_data.push_back(0x37);

        send_data.push_back(0x41);
        send_data.push_back(0x42);
        send_data.push_back(0x39);
        send_data.push_back(0x36);

        ret = computer_link.ww(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "M0640",
            2,
            send_data
        );

        std::cout << "WW command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        send_data.clear();

        send_data.push_back(0x32);
        send_data.push_back(0x33);
        send_data.push_back(0x34);
        send_data.push_back(0x37);

        send_data.push_back(0x41);
        send_data.push_back(0x42);
        send_data.push_back(0x39);
        send_data.push_back(0x36);

        ret = computer_link.qw(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "M000640",
            2,
            send_data
        );

        std::cout << "QW command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        ret = computer_link.rr(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT
        );

        std::cout << "RR command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        ret = computer_link.rs(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT
        );

        std::cout << "RS command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        ret = computer_link.gw(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            false
        );

        std::cout << "GW command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;

        ret = computer_link.gw(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            true
        );
        
        std::cout << "GW command result: " << computer_link.convert_command_status_to_string(ret) << std::endl;
    }
    catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());

        return 1;
    }


    return 0;
}
