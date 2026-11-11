/**
 * @file client.cpp
 * @brief Client implementation
 * @author Sawada
 * @date 2026-11-11
 */

#include <uds/client.hpp>
#include <uds/common/status.hpp>
#include <uds/common/io.hpp>

namespace uds {
    Client::Client(const std::string& socket_file_path)
    : socket_file_path_(socket_file_path)
    , socket_(io_context_)
    {
        ;
    }

    Client::~Client()
    {
        close();
    }

    bool Client::connect()
    {
        asio::error_code error_code;

        if (socket_.is_open()) {
            socket_.close(error_code);
        }

        asio::local::stream_protocol::endpoint ep(socket_file_path_);

        socket_.connect(ep, error_code);

        if (error_code) {
            asio::error_code close_error_code;

            socket_.close(close_error_code);

            return false;
        }

        return true;
    }

    bool Client::is_connected() const
    {
        return socket_.is_open();
    }

    void Client::close()
    {
        asio::error_code error_code;

        if (socket_.is_open()) {
            socket_.close(error_code);
        }
    }

    IOStatus Client::send(std::span<const std::uint8_t> data)
    {
        return io::send(socket_, data);
    }

    IOStatus Client::recv(std::span<std::uint8_t> buffer, std::size_t& recvd_size)
    {
        return io::recv(socket_, buffer, recvd_size);
    }

    IOStatus Client::recv_exact(std::span<std::uint8_t> buffer)
    {
        return io::recv_exact(socket_, buffer);
    }
}