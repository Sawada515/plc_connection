/**
 * @file status.cpp
 * @brief Status implementations for the UDS library
 * @author Sawada
 * @date 2026-11-11
 */

#include <uds/common/status.hpp>

namespace uds {
    IOStatus map_error_code(const asio::error_code& error_code)
    {
        if (error_code == asio::error::eof) {
            return IOStatus::PeerClosed;
        } else if (error_code == asio::error::connection_reset) {
            return IOStatus::ConnectionReset;
        } else if (error_code == asio::error::broken_pipe) {
            return IOStatus::BrokenPipe;
        } else if (error_code == asio::error::timed_out) {
            return IOStatus::Timeout;
        } else if (error_code == asio::error::interrupted) {
            return IOStatus::Interrupted;
        }

        return IOStatus::UnknownError; /* 上位でerrnoを確認したほうがいい */
    }
}
