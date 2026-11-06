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

constexpr std::uint8_t BRANCH_NUMBER = 0x00;
constexpr std::uint8_t PC_NUMBER = 0xFF;
constexpr char TELEGRAM_WAIT = '0';

int main()
{
    protocol::ComputerLink::CommandStatus ret;

    std::vector<std::uint8_t> recv_data;
    recv_data.reserve(BUFFER_SIZE);

    try {
        protocol::ComputerLink computer_link("/dev/ttyS0", false, false, 9600, 10);

        ret = computer_link.br(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "D0100",
            1,
            recv_data
        );

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
            "D0100",
            1,
            recv_data
        );

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

        ret = computer_link.br(
            BRANCH_NUMBER,
            PC_NUMBER,
            TELEGRAM_WAIT,
            "D000100",
            1,
            recv_data
        );

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
