#include <functional>
#include "epoll.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

typedef std::function<void (const std::string& req, std::string* resp)> Handler;

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
                CHECK_RET(e.Add(s, true));
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
                    getline(std::cin, *resp);
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
