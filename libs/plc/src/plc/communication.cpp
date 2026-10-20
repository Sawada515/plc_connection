/**
 * @file communication.cpp
 * @brief Implementation of the Communication class for PLC (Programmable Logic Controller) communication.
 * @author Sawada
 * @date 2026-10-21
 */

#include "plc/communication.hpp"

#include <time.h>
#include <assert.h>

#include <cerrno>

bool Communication::wait_ms(long ms)
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