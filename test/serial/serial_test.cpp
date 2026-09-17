/**
 * @file serial_test.cpp
 * @author sawada
 * @brief  Serial class test 
 * @date 2026-09-16
 * @note /dev/ttyUSB0からデータを取得しターミナルに表示
 */

#include "serial.hpp"

#include <errno.h>

#include <cstdio>

constexpr size_t BUFFER_SIZE = 256;

int main()
{
    Serial serial("/dev/ttyUSB0", Serial::BAUDRATE_9600);
    if (!serial.open()) {
        printf("Failed to open serial port\n");

        return 1;
    }

    std::uint8_t buffer[BUFFER_SIZE];

    serial.write(reinterpret_cast<const std::uint8_t*>("Serial Class Test\n"), 18);
    serial.write(reinterpret_cast<const std::uint8_t*>("Serial Class Test\n"), 18);
    serial.write(reinterpret_cast<const std::uint8_t*>("Serial Class Test\n"), 18);

    while (true) {
        ssize_t bytes_read = serial.read(buffer, BUFFER_SIZE, 1000); // 1秒のタイムアウト
        if (bytes_read < 0) {
            printf("Error reading from serial port\n");
            printf("Error code: %d\n", errno);

            break;
        }
        else if (bytes_read == 0) {
            continue;
        }

        // 読み取ったデータをターミナルに表示
        for (ssize_t i = 0; i < bytes_read; ++i) {
            printf("%c", buffer[i]);
        }
        printf("\n");
    }

    serial.close();

    return 0;
}
