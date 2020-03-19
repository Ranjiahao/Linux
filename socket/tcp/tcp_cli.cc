#include <cstdlib>
#include "tcp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcp_cli ip port" << std::endl;
        return -1;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    TcpSocket sock;
    CHECK_RET(sock.Socket());
    CHECK_RET(sock.Connect(ip, port));
    std::string buf;
    while (1) {
        std::cout << "client say: ";
        std::cin >> buf;
        CHECK_RET(sock.Send(buf));
        buf.clear();
        CHECK_RET(sock.Recv(&buf));
        std::cout << "server say: "<< buf << std::endl;
        buf.clear();
    }
    CHECK_RET(sock.Close());
    return 0;
}
