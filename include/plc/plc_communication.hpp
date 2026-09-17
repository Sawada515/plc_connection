/**
 * @file plc_comunication.hpp
 * @author sawada
 * @brief PLC Comunication header file (Serial(Computer Link) Only)
 * @date 2026-09-17
 */

#ifndef PLC_COMUNICATION_HPP_
#define PLC_COMUNICATION_HPP_

#include "serial/serial.hpp"

class PLCComunication {
  public:
    enum class Baudrate {
        BAUDRATE_300,
        BAUDRATE_600,
        BAUDRATE_1200,
        BAUDRATE_2400,
        BAUDRATE_4800,
        BAUDRATE_9600,
        BAUDRATE_19200,
        BAUDRATE_38400,
    };

    /**
     * @brief Construct a new PLC Comunication object
     * 
     * @param device_name 
     * @param baudrate 
     */
    PLCComunication(const std::string& device_name, Baudrate baudrate);

    /**
     * @brief Destroy the PLC Comunication object
     */
    ~PLCComunication();

    unsigned char br_command;

};

#endif  // PLC_COMUNICATION_HPP_