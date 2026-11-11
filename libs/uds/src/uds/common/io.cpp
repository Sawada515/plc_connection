/**
 * @file io.cpp
 * @brief I/O implementations for the UDS library
 * @author Sawada
 * @date 2026-11-11
 */

#include <uds/common/io.hpp>

namespace uds::io {
    uds::IOStatus send(
        asio::local::stream_protocol::socket& socket,
        std::span<const std::uint8_t> data
    )
    {
        if (!socket.is_open()) {
            return uds::IOStatus::IsNotConnected;
        }

        asio::error_code error_code;

        asio::write(socket, asio::buffer(data, data.size()), error_code);

        if (error_code) {
            if (error_code == asio::error::eof) {
                asio::error_code close_error_code;

                socket.close(close_error_code);
            }

            return map_error_code(error_code);
        }

        return uds::IOStatus::Success;
    }

    uds::IOStatus recv(
        asio::local::stream_protocol::socket& socket,
        std::span<std::uint8_t> buffer, std::size_t& recvd_size
    )
    {
        recvd_size = 0;

        if (!socket.is_open()) {
            return uds::IOStatus::IsNotConnected;
        }

        asio::error_code error_code;

        std::size_t recved_size = socket.read_some(asio::buffer(buffer, buffer.size()), error_code);

        if (error_code) {
            if (error_code == asio::error::eof) {
                asio::error_code close_error_code;

                socket.close(close_error_code);

                return map_error_code(error_code);
            }

            return map_error_code(error_code);
        }

        recvd_size = recved_size;

        return uds::IOStatus::Success;
    }

    uds::IOStatus recv_exact(
        asio::local::stream_protocol::socket& socket,
        std::span<std::uint8_t> buffer
    )
    {
        if (!socket.is_open()) {
            return uds::IOStatus::IsNotConnected;
        }

        asio::error_code error_code;

        std::size_t recv_size = asio::read(socket, asio::buffer(buffer, buffer.size()), error_code);

        if (error_code) {
            if (error_code == asio::error::eof) {
                asio::error_code close_error_code;

                socket.close(close_error_code);

                return map_error_code(error_code);
            }

            return map_error_code(error_code);
        }

        return (recv_size == buffer.size())
            ? uds::IOStatus::Success : map_error_code(error_code);
    }
}