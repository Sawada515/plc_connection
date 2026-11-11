/**
 * @file server.hpp
 * @brief Server implementation
 * @author Sawada
 * @date 2026-11-11
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif // ASIO_STANDALONE

#include <asio.hpp>

#include <string>
#include <cstdint>
#include <span>

#include <uds/common/status.hpp>

namespace uds {
    class Server {
      public:
        explicit Server(const std::string& socket_file_path);
        ~Server();

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

        bool accept();

        void close();

        bool is_connected() const;

        IOStatus send(std::span<const std::uint8_t> data);

        IOStatus recv(std::span<std::uint8_t> buffer, std::size_t& recv_size);
        IOStatus recv_exact(std::span<std::uint8_t> buffer);

      private:
        std::string socket_file_path_;

        asio::io_context io_context_;
        asio::local::stream_protocol::acceptor acceptor_;
        asio::local::stream_protocol::socket socket_;
    };
}

#endif // SERVER_HPP