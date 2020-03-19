#include <cstdlib>
#include "tcp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

struct Information {
    TcpSocket sock;
    std::string ip;
    uint16_t port;
};

void* thr_start(void* arg) {
    Information* info = (Information*)arg;
    std::string buf;
    while (1) {
        if (!info->sock.Recv(&buf)) {
            info->sock.Close();
            delete info;
            return NULL;
        }
        std::cout << "client[" << info->ip << ":" << info->port << "]say: " << buf << std::endl;
        buf.clear();
        std::cout << "server say: ";
        std::cin >> buf;
        if (!info->sock.Send(buf)) {
            info->sock.Close();
            delete info;
            return NULL;
        }
        buf.clear();
    }
    info->sock.Close();
    delete info;
    return NULL;
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
    while (1) {
        // 再堆上申请空间防止析构
        Information* info = new Information();
        if (!lst_sock.Accept(&info->sock, &info->ip, &info->port)) {
            delete info;
            continue;
        }
        // 创建新线程与客户端通信
        pthread_t tid;
        pthread_create(&tid, NULL, thr_start, (void*)info);
        pthread_detach(tid);
    }
    CHECK_RET(lst_sock.Close());
    return 0;
}
