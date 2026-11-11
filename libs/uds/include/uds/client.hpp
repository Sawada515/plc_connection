/**
 * @file client.hpp
 * @brief Client implementation
 * @author Sawada
 * @date 2026-11-11
 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif // ASIO_STANDALONE

#include <asio.hpp>

#include <string>
#include <cstdint>
#include <span>

#include <uds/common/status.hpp>

namespace uds {
    class Client {
      public:
        explicit Client(const std::string& socket_file_path);
        ~Client();

        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;
        Client(Client&&) = delete;
        Client& operator=(Client&&) = delete;

        bool connect();

        void close();

        bool is_connected() const;

        IOStatus send(std::span<const std::uint8_t> data);

        IOStatus recv(std::span<std::uint8_t> buffer, std::size_t& recv_size);
        IOStatus recv_exact(std::span<std::uint8_t> buffer);

      private:
        std::string socket_file_path_;

        asio::io_context io_context_;
        asio::local::stream_protocol::socket socket_;
    };
}

#endif // CLIENT_HPP