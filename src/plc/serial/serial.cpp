/**
 * @file serial.cpp
 * @brief Implementation of the Serial class for PLC (Programmable Logic Controller) serial communication.
 * @author Sawada
 * @date 2026-09-13
 */

#include "serial.hpp"

#include <unistd.h>
#include <poll.h>
#include <error.h>
#include <string.h>

#include <cstring>
#include <chrono>

Serial::Serial(const std::string& device_name, baudrate_t baudrate)
    : device_name_(device_name)
    , baudrate_(baudrate)
    , fd_(-1)
    {
        ;
    }

Serial::~Serial()
{
    close();
}

bool Serial::open()
{
    fd_ = ::open(device_name_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd_ == -1) {
        return false;
    }

    struct termios tty{};
    cfsetospeed(&tty, baudrate_);
    cfsetispeed(&tty, baudrate_);

    tty.c_cflag &= ~PARENB; // parity none
    tty.c_cflag &= ~CSTOPB; // stop bit 1
    tty.c_cflag &= ~CSIZE;  // clear size bits
    tty.c_cflag |= CS8;     // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // no flow control
    tty.c_cflag |= CREAD | CLOCAL; // enable receiver, ignore modem control

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // disable canonical mode
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST; // disable output processing

    tty.c_cc[VTIME] = 2;
    tty.c_cc[VMIN] = 1;

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        return false;
    }

    return true;
}

void Serial::close()
{
    if (fd_ != -1) {
        ::close(fd_);

        fd_ = -1;
    }
}

bool Serial::write(const std::uint8_t *data, size_t size)
{
    if (fd_ == -1 || data == nullptr || size <= 0) {
        return false;
    }

    while (size > 0) {
        ssize_t wrote_bytes = ::write(fd_, data, size);
        if (wrote_bytes <= 0) {
            return false;
        }

        data += wrote_bytes;
        size -= wrote_bytes;
    }

    return true;
}

ssize_t Serial::read(std::uint8_t* buffer, const size_t max_read_size, const int timeout_ms)
{
    size_t already_read_byte = 0;

    if (fd_ == -1 || buffer == nullptr || max_read_size == 0 || timeout_ms == 0) {
        return -1;
    }

    struct pollfd pfd = {
        .fd = fd_,
        .events = POLLIN,
        .revents = 0
    };

    const auto start_time_ms = std::chrono::steady_clock::now();

    while (already_read_byte < max_read_size) {
        const auto current_time_ms = std::chrono::steady_clock::now();

        // 経過時間
        const auto elapsed_time_ms = 
            std::chrono::duration_cast<std::chrono::milliseconds>(current_time_ms - start_time_ms).count();
        
        if (elapsed_time_ms >= timeout_ms) {
            break;  // timeout
        }

        const int remaining_time = timeout_ms - static_cast<int>(elapsed_time_ms);

        int ret = ::poll(&pfd, 1, remaining_time);        
        if (ret < 0) {
            if (errno == EINTR) {
                continue; 
            }
            
            return -1;
        }

        if (ret == 0) {
            break;  // timeout
        }

        // POLLERR  <= エラー状態
        // POLLHUP  <= 切断もしくは読み取り可能状態ではない
        // POLLNVAL <= 無効なファイルディスクリプタ
        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            return -1;
        }

        // POLLIN <= 読み取り可能データあり
        if (pfd.revents & POLLIN) {
            size_t n = ::read(fd_, buffer + already_read_byte, max_read_size - already_read_byte);
            if (n < 0) {
                if (errno == EINTR) {
                    continue; 
                }
    
                return -1;
            }
    
            if (n == 0) {
                continue;
            }
    
            already_read_byte += static_cast<size_t>(n);
        }
    }

    return static_cast<ssize_t>(already_read_byte);
}