/**
 * @file serial.hpp
 * @author Sawada
 * @brief 
 * @version 0.1
 * @date 2026-09-18
 * 
 * @copyright Copyright (c) 2026
 */

#ifndef SERIAL_HPP_
#define SERIAL_HPP_

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <string>
#include <cstdint>

class Serial {
  public:
    enum class baudrate_t : int {
        BAUDRATE_300 = B300,
        BAUDRATE_600 = B600,
        BAUDRATE_1200 = B1200,
        BAUDRATE_2400 = B2400,
        BAUDRATE_4800 = B4800,
        BAUDRATE_9600 = B9600,
        BAUDRATE_19200 = B19200,
        BAUDRATE_38400 = B38400,
    };

    /**
     * @brief Constructor
     * 
     * @param[in] device_name 
     * @param[in] baudrate 
     * @param[in] timeout_ms 
     * @brief  timeout_msはPLC側と値を合わせる
     */
    Serial(const std::string& device_name, baudrate_t baudrate, int timeout_ms);

    /**
     * @brief Destructor
     */
    ~Serial();

    /**
     * @brief Open the serial port
     * 
     * @return true 
     * @return false 
     */
    bool open();

    /**
     * @brief Close the serial port
     */
    void close();

    /**
     * @brief Write data to the serial port
     * 
     * @param[in] data 
     * @param[in] size 
     * @return true 
     * @return false 
     */
    bool write(const std::uint8_t* data, size_t size);

    /**
     * @brief Read data from the serial port
     * 
     * @param[inout] buffer 
     * @param[in] max_read_size 
     * @param[in] timeout_ms 
     * @return ssize_t 
     * 
     * @note timeout_ms: recive data timeout
     */
    ssize_t read(std::uint8_t* buffer, const size_t max_read_size);

  private:
    std::string device_name_;
    baudrate_t baudrate_;
    int timeout_ms_;
    int fd_;
};

#endif  // SERIAL_HPP_