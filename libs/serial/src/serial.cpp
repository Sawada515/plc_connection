/**
 * @file serial.hpp
 * @author Sawada
 * @brief 
 * @version 0.1
 * @date 2026-10-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <serial/serial.hpp>

#include <unistd.h>
#include <poll.h>
#include <error.h>
#include <string.h>
#include <assert.h>

#include <cstring>
#include <chrono>

Serial::Serial(const std::string& device_name, baudrate_t baudrate, int timeout_ms)
    : device_name_(device_name)
    , baudrate_(baudrate)
    , timeout_ms_(timeout_ms)
    , fd_(-1)
    {
        assert(timeout_ms_ > 0);
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

    /*　既存既存設定を読み出し */
    if (tcgetattr(fd_, &tty) != 0) {
        ::close(fd_);

        fd_ = -1;

        return false;
    }

    // Raw mode
    cfmakeraw(&tty);

    // set bandrate
    cfsetospeed(&tty, static_cast<speed_t>(baudrate_));
    cfsetispeed(&tty, static_cast<speed_t>(baudrate_));

    tty.c_cflag &= ~PARENB; // parity none
    tty.c_cflag &= ~CSTOPB; // stop bit 1
    tty.c_cflag &= ~CSIZE;  // clear size bits
    tty.c_cflag |= CS8;     // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // no flow control
    tty.c_cflag |= CREAD | CLOCAL; // enable receiver, ignore modem control

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 1;

    tcflush(fd_, TCIOFLUSH);

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        return false;
    }

    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags != -1) {
        fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
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

    if (::tcdrain(fd_) != 0) {
        return false;
    }

    ::tcflush(fd_, TCIFLUSH);

    return true;
}

ssize_t Serial::read(std::uint8_t* buffer, const size_t max_read_size)
{
    if (fd_ == -1 || buffer == nullptr || max_read_size == 0 || timeout_ms_ == 0) {
        return -1;
    }

    struct pollfd pfd = {
        .fd = fd_,
        .events = POLLIN,
        .revents = 0
    };

    int ret = ::poll(&pfd, 1, timeout_ms_);
    if (ret < 0) {
        if (errno == EINTR) return 0;
        return -1;
    }
    if (ret == 0) {
        return 0; // タイムアウト
    }

    if (pfd.revents & POLLIN) {
        return ::read(fd_, buffer, max_read_size);
    }

    return -1;
}
