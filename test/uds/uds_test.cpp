/**
 * @file uds_test.cpp
 * @brief UDS test implementation
 * @author Sawada
 * @date 2026-11-11
 */

#include <uds/server.hpp>
#include <uds/client.hpp>

#include <thread>
#include <iostream>
#include <string>
#include <cstdint>

void server_thread();

int main()
{
    std::thread server(server_thread);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    uds::Client client("/tmp/test.socket");

    client.connect();

    std::string message = "Test message from client";

    client.recv_exact(std::span<std::uint8_t>(
        reinterpret_cast<std::uint8_t*>(message.data()),
        message.size()
    ));

    std::cout << "Received message from server: " << message << std::endl;

    message = "Test message from client";

    client.send(std::span<const std::uint8_t>(
        reinterpret_cast<const std::uint8_t*>(message.data()),
        message.size()
    ));

    client.close();

    server.join();

    return 0;

}

void server_thread()
{
    uds::Server server("/tmp/test.socket");

    server.accept();

    std::string message = "Test message from server";

    server.send(std::span<const std::uint8_t>(
        reinterpret_cast<const std::uint8_t*>(message.data()),
        message.size()
    ));

    server.recv_exact(std::span<std::uint8_t>(
        reinterpret_cast<std::uint8_t*>(message.data()),
        message.size()
    ));

    std::cout << "Received message from client: " << message << std::endl;

    server.close();

    return;
}