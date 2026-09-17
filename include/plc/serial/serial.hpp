/**
 * @file serial.hpp
 * @brief Serial communication interface for PLC (Programmable Logic Controller).
 * @author Sawada
 * @date 2026-09-13
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
    typedef enum baudrate {
        BAUDRATE_300 = B300,
        BAUDRATE_600 = B600,
        BAUDRATE_1200 = B1200,
        BAUDRATE_2400 = B2400,
        BAUDRATE_4800 = B4800,
        BAUDRATE_9600 = B9600,
        BAUDRATE_19200 = B19200,
        BAUDRATE_38400 = B38400,
    } baudrate_t;

    /**
     * @brief Constructor
     * 
     * @param device_name 
     * @param baudrate 
     */
    Serial(const std::string& device_name, baudrate_t baudrate);

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
    ssize_t read(std::uint8_t* buffer, const size_t max_read_size, const int timeout_ms);

  private:
    std::string device_name_;
    baudrate_t baudrate_;
    int fd_;
};

#endif  // SERIAL_HPP_