/**
 * @file computer_link_test.cpp
 * @author sawada
 * @brief  computer link class test 
 * @date 2026-11-06
 * @note 計算機リンクプロトコルの動作確認
 */

#include <protocol/computer_link.hpp>

#include <errno.h>

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <stdexcept>

constexpr size_t BUFFER_SIZE = 256;

constexpr std::uint8_t BRANCH_NUMBER = 0x05;
constexpr std::uint8_t PC_NUMBER = 0xFF;
constexpr char TELEGRAM_WAIT = '0';

int main()
{
    protocol::ComputerLink::CommandStatus ret;

    std::vector<std::uint8_t> recv_data;
    recv_data.reserve(BUFFER_SIZE);

    try {
        // チェックサムなし　CRLFなし   OK
        //protocol::ComputerLink computer_link("/dev/ttyUSB0", false, false, 5, 10);
        // チェックサムあり　CRLFなし   OK
        //protocol::ComputerLink computer_link("/dev/ttyUSB0", true, false, 5, 10);
        // チェックサムなし　CRLFあり   OK
        //protocol::ComputerLink computer_link("/dev/ttyUSB0", false, true, 5, 10);
        // チェックサムあり CRLFあり
        protocol::ComputerLink computer_link("/dev/ttyUSB0", true, true, 5, 10);

        ret = computer_link.br(
            BRANCH_NUMBER,
            PC_NUMBER,
            'A',
            "X0040",
            5,
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
            "X0040",
            2,
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
            "X000040",
            2,
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
    }
    catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());

        return 1;
    }

    return 0;
}
