/**
 * @file network.hpp
 * @brief Provides platform-independent networking types and socket abstractions.
 */

#pragma once

#define ASIO_STANDALONE

#include <platform/types.hpp>
#include <asio.hpp>


/**
 * @namespace Windy::Platform
 * @brief Contains low-level hardware abstraction and platform-specific logic
 */
namespace Windy::Platform {
    using Port = std::uint16_t;

    /**
     * @class NetContext
     * @brief Wraps the asio::io_context to manage asynchronous I/O operations.
     */
    class NetContext {
    public:
        NetContext() : work(asio::make_work_guard(io)) {}

        /**
         * @brief Processes pending network events without blocking
         * @note Call this once per frame in the main engine loop
         */
        void poll();

        /**
         * @brief Returns the underlying asio context
         */
        asio::io_context& getRaw();
    private:
        asio::io_context io;
        asio::executor_work_guard<asio::io_context::executor_type> work;
    };

    /**
     * @class UDPSocket
     * @brief A high-speed, connectionless socket for gameplay data.
     */
    class UDPSocket {
    public:
        explicit UDPSocket(NetContext& context)
            : socket(context.getRaw(), asio::ip::udp::v4()) {};

        /**
         * @brief Binds the socket to a specific port for listening.
         */
        void bind(Port port);
    private:
        asio::ip::udp::socket socket;
    };
}