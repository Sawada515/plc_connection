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
#include <string>

class Communication {
  public:
    /**
     * @brief Construct a new Communication object
     */
    Communication();

    /**
     * @brief Destroy the Communication object
     */
    ~Communication();

    bool read_bit_device(std::string &device_address, )



  private:
    /**
     * @brief Calculate the checksum of the given data
     * 
     * @param[in] data Pointer to the data
     * @param[in] length Length of the data
     * @return std::uint16 The calculated checksum
     */
    std::uint16_t calculate_checksum(const std::uint8_t* data, size_t length);

    /**
     * @brief Wait for a specified number of milliseconds
     * 
     * @param[in] ms Number of milliseconds to wait
     * @return true if the wait was successful
     * @return false if an error occurred during the wait
     */
    bool wait_ms(long ms);
};