/**
 * @file server.cpp
 * @brief Server implementation
 * @author Sawada
 * @date 2026-11-11
 */

#include <uds/server.hpp>
#include <uds/common/status.hpp>
#include <uds/common/io.hpp>

namespace uds {
    Server::Server(const std::string& socket_file_path)
    : socket_file_path_(socket_file_path)
    , acceptor_(io_context_)
    , socket_(io_context_)
    {
        ;
    }

    Server::~Server()
    {
        close();
    }

    bool Server::accept()
    {
        asio::error_code error_code;

        if (socket_.is_open()) {
            socket_.close(error_code);
        }
        if (acceptor_.is_open()) {
            acceptor_.close(error_code);
        }

        ::unlink(socket_file_path_.c_str());

        asio::local::stream_protocol::endpoint ep(socket_file_path_);

        acceptor_.open(ep.protocol(), error_code);
        if (error_code) {
            close();

            return false;
        }

        acceptor_.bind(ep, error_code);
        if (error_code) {
            return false;
        }

        acceptor_.listen(asio::socket_base::max_listen_connections, error_code);
        if (error_code) {
            return false;
        }

        acceptor_.accept(socket_, error_code);

        return !error_code;
    }

    void Server::close()
    {
        asio::error_code error_code;

        if (socket_.is_open()) {
            socket_.close(error_code);
        }
        if (acceptor_.is_open()) {
            acceptor_.close(error_code);
        }

        ::unlink(socket_file_path_.c_str());
    }

    IOStatus Server::send(std::span<const std::uint8_t> data)
    {
        return io::send(socket_, data);
    }

    IOStatus Server::recv(std::span<std::uint8_t> buffer, std::size_t& recvd_size)
    {
        return io::recv(socket_, buffer, recvd_size);
    }

    IOStatus Server::recv_exact(std::span<std::uint8_t> buffer)
    {
        return io::recv_exact(socket_, buffer);
    }
}