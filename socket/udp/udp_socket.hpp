// 封装一个UdpSocket类，向外提供更容易使用的udp接口来实现udp通信流程

#include <cstdio>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

class UdpSocket {
public:
    UdpSocket()
        : _sockfd(-1) {}

    ~UdpSocket() {
        Close();
    }

    bool Socket() {
        _sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (_sockfd < 0) {
            perror("socket");
            return false;
        }
        return true;
    }

    bool Bind(const std::string& ip, const uint16_t port) const {
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

    bool Recv(std::string* buf, std::string* ip = NULL, uint16_t* port = NULL) const {
        char tmp[4096] = { 0 };
        sockaddr_in peer_addr;
        socklen_t len = sizeof(peer_addr);
        int ret = recvfrom(_sockfd, tmp, 4096, 0, (sockaddr*)&peer_addr, &len);
        if (ret < 0) {
            perror("recvfrom");
            return false;
        }
        if (ip) {
            *ip = inet_ntoa(peer_addr.sin_addr);
        }
        if (port) {
            *port = ntohs(peer_addr.sin_port);
        }
        *buf = std::string(tmp, ret);
        return true;
    }

    bool Send(const std::string& data, const std::string& ip, const uint16_t port) const {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(&ip[0]);
        socklen_t len = sizeof(sockaddr_in);
        if (sendto(_sockfd, &data[0], data.size(), 0, (sockaddr*)&addr, len) < 0) {
            perror("sendto");
            return false;
        }
        return true;
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
