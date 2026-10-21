/**
 * @file computer_link.cpp
 * @author Sawada
 * @brief 
 * @version 0.1
 * @date 2026-10-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "protocol/computer_link.hpp"

bool ComputerLink::wait_ms(long ms)
{
    assert(ms >= 0);

    timespec req{};

    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000L;

    while (true) {
        int ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &req, &req);
        if (ret == 0) {
            return true;
        }
        if (ret != EINTR) {
            return false;
        }
    }
}