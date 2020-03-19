#include <cstdlib>
#include "tcp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcp_srv ip port" << std::endl;
        return -1;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    TcpSocket lst_sock;
    CHECK_RET(lst_sock.Socket());
    CHECK_RET(lst_sock.Bind(ip, port));
    CHECK_RET(lst_sock.Listen());
    std::string buf;
    while (1) {
        TcpSocket newsock;
        if (!lst_sock.Accept(&newsock, &ip, &port)) {
            continue;
        }
        if (!newsock.Recv(&buf)) {
            continue;
        }
        std::cout << "client[" << ip << ":" << port << "]say: " << buf << std::endl;
        buf.clear();
        std::cout << "server say: ";
        std::cin >> buf;
        newsock.Send(buf);
        buf.clear();
        CHECK_RET(newsock.Close());
    }
    CHECK_RET(lst_sock.Close());
    return 0;
}
