#include <platform/network.hpp>


using namespace Windy::Platform;

void NetContext::poll() {
    io.poll();
}

asio::io_context& NetContext::getRaw() {
    return io;
}

void UDPSocket::bind(Port port) {
    socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
}