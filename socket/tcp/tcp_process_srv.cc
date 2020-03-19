#include <cstdlib>
#include <signal.h>
#include <sys/wait.h>
#include "tcp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

void sigcb(int signo) {
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcp_srv ip port" << std::endl;;
        return -1;
    }
    signal(SIGCHLD, sigcb);
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    TcpSocket lst_sock;
    CHECK_RET(lst_sock.Socket());
    CHECK_RET(lst_sock.Bind(ip, port));
    CHECK_RET(lst_sock.Listen());
    TcpSocket newsock;
    while (1) {
        if (!lst_sock.Accept(&newsock, &ip, &port)) {
            continue;
        }
        // 创建子进程处理任务
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
        } else if (pid == 0) {
            // 子进程负责与客户端进行通信
            // 1. 任务分摊--每个进程只负责一个任务，主进程负责获取新连接
            // 2. 稳定性高--子进程处理请求出问题不会影响到主进程服务器
            std::string buf;
            while (1) {
                if (!newsock.Recv(&buf)) {
                    newsock.Close();
                    exit(-1);
                }
                std::cout << "client[" << ip << ":" << port << "]say: " << buf << std::endl;
                buf.clear();
                std::cout << "server say: ";
                std::cin >> buf;
                if (!newsock.Send(buf)) {
                    newsock.Close();
                    exit(-1);
                }
                buf.clear();
            }
            CHECK_RET(newsock.Close());
            exit(0);
        } else {
            CHECK_RET(newsock.Close()); // 父进程关闭自己的不用的socket不影响子进程
        }
    }
    CHECK_RET(lst_sock.Close());
    return 0;
}
