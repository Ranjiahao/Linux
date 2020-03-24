#include <sys/epoll.h>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "tcp_socket.hpp"

#define MAX_EPOLL 1024
#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

typedef std::function<void (const std::string& req, std::string* resp)> Handler;

class Epoll {
public:
    Epoll()
        : _epfd(-1) {
        }

    bool Init() {
        _epfd = epoll_create(MAX_EPOLL);
        if (_epfd < 0) {
            perror("epoll_create");
            return false;
        }
        return true;
    }

    bool Add(const TcpSocket& sock) const {
        struct epoll_event ev;
        int fd = sock.GetFd();
        ev.events = EPOLLIN;
        // ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = fd;
        int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
        if (ret < 0) {
            perror("epoll_ctl ADD");
            return false;
        }
        return true;
    }

    bool Del(const TcpSocket& sock) const {
        int fd = sock.GetFd();
        int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
        if (ret < 0) {
            perror("epoll_ctl DEL");
            return false;
        }
        return true;
    }

    bool Wait(std::vector<TcpSocket>* output) const {
        output->clear();
        struct epoll_event evs[MAX_EPOLL];
        int nfds = epoll_wait(_epfd, evs, MAX_EPOLL, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            return false;
        }
        for (int i = 0; i < nfds; ++i) {
            if (evs[i].events & EPOLLIN) {
                TcpSocket sock(evs[i].data.fd);
                output->push_back(sock);
            }
        }
        return true;
    }
private:
    int _epfd;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcp_epoll ip port" << std::endl;
        return -1;
    }
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    TcpSocket lst_sock;
    CHECK_RET(lst_sock.Socket());
    CHECK_RET(lst_sock.Bind(ip, port));
    CHECK_RET(lst_sock.Listen());
    CHECK_RET(lst_sock.SetNonBlock());
    Epoll e;
    CHECK_RET(e.Init());
    CHECK_RET(e.Add(lst_sock));
    while (1) {
        std::vector<TcpSocket> output;
        if (e.Wait(&output) == false) {
            continue;
        }
        for (auto& clisock : output) {
            if (lst_sock.GetFd() == clisock.GetFd()) {
                TcpSocket s;
                if (lst_sock.Accept(&s) == false) {
                    continue;
                }
                CHECK_RET(s.SetNonBlock());
                CHECK_RET(e.Add(s));
            } else {
                std::string req, resp;
                if (clisock.Recv(&req) == false) {
                    e.Del(clisock);
                    clisock.Close();
                    continue;
                }
                Handler handler = [](const std::string& req, std::string* resp) {
                    std::cout << "client say: " << req << std::endl;   
                    std::cout << "server say: ";
                    std::cin >> *resp;
                };
                handler(req, &resp);
                if (clisock.Send(resp) == false) {
                    e.Del(clisock);
                    clisock.Close();
                    continue;
                }
            }
        }
    }
    CHECK_RET(lst_sock.Close());
    return 0;
}
