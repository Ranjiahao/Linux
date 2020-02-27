#include <iostream>
#include <cstdlib>
#include "udp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./udp_srv ip port" << std::endl;
        return -1;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    UdpSocket sock;
    CHECK_RET(sock.Socket());
    CHECK_RET(sock.Bind(ip, port));
    std::string buf;
    while(1) {
        std::string peer_ip;
        uint16_t peer_port;
        CHECK_RET(sock.Recv(&buf, &peer_ip, &peer_port));
        std::cout << "client[" << peer_ip << ":" << peer_port << "]say: " << buf << std::endl;
        buf.clear();
        std::cout << "server say: ";
        std::cin >> buf;
        CHECK_RET(sock.Send(buf, peer_ip, peer_port));
        buf.clear();
    }
    return 0;
}
