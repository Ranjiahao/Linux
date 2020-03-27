// 封装一个TcpSocket类，向外提供更容易使用的tcp接口来实现tcp通信流程

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <string>

#define MAX_LISTEN 5

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

class TcpSocket {
public:
    TcpSocket(int i = -1)
        : _sockfd(i) {}

    bool Socket() {
        _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_sockfd < 0) {
            perror("socket");
            return false;
        }
        return true;
    }

    bool Bind(const std::string& ip, uint16_t port) const {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(&ip[0]);
        socklen_t len = sizeof(sockaddr_in);
        if (bind(_sockfd, (sockaddr*)&addr, len) < 0) {
            perror("bind");
            return false;
        }
        return true;
    }

    bool Listen(int backlog = MAX_LISTEN) const {
        if (listen(_sockfd, backlog) < 0) {
            perror("listen");
            return false;
        }
        return true;
    }

    bool Connect(const std::string& ip, uint16_t port) const {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(&ip[0]);
        socklen_t len = sizeof(sockaddr_in);
        if (connect(_sockfd, (sockaddr*)&addr, len) < 0) {
            perror("connect");
            return false;
        }
        return true;
    }

    bool Accept(TcpSocket* new_sock, std::string* ip = NULL, uint16_t* port = NULL) const {
        sockaddr_in addr;
        socklen_t len = sizeof(sockaddr_in);
        int new_fd = accept(_sockfd, (sockaddr*)&addr, &len);
        if (new_fd < 0) {
            perror("accept");
            return false;
        }
        if (ip) {
            *ip = inet_ntoa(addr.sin_addr);
        }
        if (port) {
            *port = ntohs(addr.sin_port);
        }
        new_sock->_sockfd = new_fd;
        return true;
    }

    bool Send(std::string& buf) const {
        if (send(_sockfd, &buf[0], buf.size(), 0) < 0) {
            perror("send");
            return false;
        }
        return true;
    }

    bool SendNoBlock(const std::string& buf) const {
        int cur = 0; // 当前写到的位置
        int left = buf.size();
        while (1) {
            int write_size = send(_sockfd, &buf[0] + cur, left, 0);
            if (write_size < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // TCP发送缓冲区满了
                    continue;
                }
                perror("send");
                return false;
            }
            cur += write_size;
            left -= write_size;
            if (left <= 0) {
                // 写完数据
                break;
            }
        }
        return true;
    }

    bool Recv(std::string* buf) const {
        char tmp[4096] = { 0 };
        int ret = recv(_sockfd, tmp, 4095, 0);
        if (ret < 0) {
            perror("recv");
            return false;
        } else if (ret == 0) {
            // 连接已经断开
            std::cerr << "peer shutdown" << std::endl;
            return false;
        }
        *buf = std::string(tmp, ret);
        return true;
    }
    
    bool RecvNoBlock(std::string* buf) const {
        char tmp[4096] = { 0 };
        while (1) {
            int ret = recv(_sockfd, tmp, 4095, 0);
            if (ret < 0) {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // TCP接收缓冲区为空
                    return true;
                }
                perror("recv");
                return false;
            } else if (ret == 0) {
                // 连接已经断开
                std::cerr << "peer shutdown" << std::endl;
                return false;
            }
            tmp[ret] = '\0';
            *buf += tmp;
        }
    }

    bool SetNonBlock() const {
        int flag = fcntl(_sockfd, F_GETFL, 0);
        if (flag < 0) {
            perror("fcntl");
            return false;
        }
        if (fcntl(_sockfd, F_SETFL, O_NONBLOCK | flag) < 0) {
            perror("fcntl");
            return false;
        }
        return true;
    }

    int GetFd() const {
        return _sockfd;
    }

    bool Close() {
        if (_sockfd >= 0) {
            if (close(_sockfd) < 0) {
                perror("close");
                return false;
            }
            _sockfd = -1;
        }
        return true;
    }
private:
    int _sockfd;
};
