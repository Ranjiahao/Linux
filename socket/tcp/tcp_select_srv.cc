#include <vector>
#include <unordered_map>
#include "tcp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

class Select {
public:
    Select()
        : _maxfd(-1) {
            FD_ZERO(&_rfds);
        }

    bool Add(const TcpSocket& sock) {
        int fd = sock.GetFd();
        if (_fdmap.count(fd)) {
            return false;
        }
        _fdmap[fd] = sock;
        FD_SET(fd, &_rfds);
        _maxfd = _maxfd > fd ? _maxfd : fd;
        return true;
    }

    bool Del(const TcpSocket& sock) {
        int fd = sock.GetFd();
        if (!_fdmap.count(fd)) {
            return false;
        }
        _fdmap.erase(fd);
        FD_CLR(fd, &_rfds);
        for (int i = _maxfd; i >=0; --i) {
            if (FD_ISSET(i, &_rfds)) {
                _maxfd = i;
                return true;
            }
        }
        _maxfd = -1;
        return true;
    }

    // 返回读就绪的文件描述符集
    bool Wait(std::vector<TcpSocket>* output) {
        output->clear();
        // 避免监控集合被修改，避免每次重新向集合中添加
        fd_set tmp_set = _rfds;
        int nfds = select(_maxfd + 1, &tmp_set, NULL, NULL, NULL);
        if (nfds < 0) {
            perror("select");
            return false;
        }
        for (int i = 0; i <= _maxfd; ++i) {
            if (FD_ISSET(i, &tmp_set)) {
                output->push_back(_fdmap[i]);
            }
        }
        return true;
    }
private:
    fd_set _rfds;
    int _maxfd;
    // 文件描述符和socket的映射
    std::unordered_map<int, TcpSocket> _fdmap;
};

int main(int argc, char *argv[]) {
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
    Select selector;
    selector.Add(lst_sock);
    while (1) {
        std::vector<TcpSocket> output;
        if (selector.Wait(&output) == false) {
            continue;
        }
        for (auto& clisock : output) {
            if (clisock.GetFd() == lst_sock.GetFd()) {
                // 当前就绪的描述符是监听套接字，应该accept
                TcpSocket s;
                if (lst_sock.Accept(&s) == false) {
                    continue;
                }
                selector.Add(s);
            } else {
                // 当前就绪的描述符是通信套接字，应该recv
                std::string buf;
                if (clisock.Recv(&buf) == false) {
                    selector.Del(clisock);
                    clisock.Close();
                    continue;
                }
                std::cout << "client say: " << buf << std::endl;
                buf.clear();
                std::cout << "server say: ";
                std::cin >> buf;
                if (clisock.Send(buf) == false) {
                    selector.Del(clisock);
                    clisock.Close();
                    continue;
                }
            }
        }
    }
    CHECK_RET(lst_sock.Close());
    return 0;
}
