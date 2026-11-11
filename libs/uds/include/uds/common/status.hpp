/**
 * @file status.hpp
 * @brief Status definitions for the UDS library
 * @author Sawada
 * @date 2026-11-11
 */

#ifndef STATUS_HPP
#define STATUS_HPP

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif // ASIO_STANDALONE

#include <asio.hpp>

namespace uds {
    enum class IOStatus {
        Success,
        IsNotConnected,
        PeerClosed,
        ConnectionReset,
        BrokenPipe,
        Timeout,
        Interrupted,
        UnknownError
    };

    IOStatus map_error_code(const asio::error_code& error_code);
}

#endif // STATUS_HPP