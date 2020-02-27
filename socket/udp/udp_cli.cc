#include <iostream>
#include <cstdlib>
#include "udp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./udp_cli ip port" << std::endl;
        return -1;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    UdpSocket sock;
    CHECK_RET(sock.Socket());
    std::string buf;
    while(1) {
        std::cout << "client say: ";
        std::cin >> buf;
        CHECK_RET(sock.Send(buf, ip, port));
        buf.clear();
        CHECK_RET(sock.Recv(&buf));
        std::cout << "server say: " << buf << std::endl;
        buf.clear();
    }
    return 0;
}
