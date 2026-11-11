/**
 * @file io.hpp
 * @brief I/O implementations for the UDS library
 * @author Sawada
 * @date 2026-11-11
 */

#ifndef IO_HPP
#define IO_HPP

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif // ASIO_STANDALONE

#include <asio.hpp>

#include <uds/common/status.hpp>

namespace uds::io {
    uds::IOStatus send(
        asio::local::stream_protocol::socket& socket,
        std::span<const std::uint8_t> data
    );

    uds::IOStatus recv(
        asio::local::stream_protocol::socket& socket,
        std::span<std::uint8_t> buffer,
        std::size_t& recv_size
    );

    uds::IOStatus recv_exact(
        asio::local::stream_protocol::socket& socket,
        std::span<std::uint8_t> buffer
    );
}

#endif // IO_HPP