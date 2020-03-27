#include <sys/epoll.h>
#include <vector>
#include "tcp_socket.hpp"

#define MAX_EPOLL 1024

class Epoll {
public:
    Epoll()
        : _epfd(-1) {
        }

    ~Epoll() {
        close(_epfd);
    }

    bool Init() {
        _epfd = epoll_create(MAX_EPOLL);
        if (_epfd < 0) {
            perror("epoll_create");
            return false;
        }
        return true;
    }

    bool Add(const TcpSocket& sock, bool epoll_et = false) const {
        epoll_event ev;
        int fd = sock.GetFd();
        ev.data.fd = fd;
        if (epoll_et) {
            ev.events = EPOLLIN | EPOLLET;
        } else {
            ev.events = EPOLLIN;
        }
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
