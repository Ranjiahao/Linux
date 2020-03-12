// 实现一个简单的http服务器，不管客户端发出什么请求，服务器都响应一个hello world 页面
#include <sstream>
#include "tcp_socket.hpp"

#define CHECK_RET(q) \
    if ((q) == false) { \
        return -1; \
    }

int main(int argc, char* argv[]) {
    TcpSocket sock;
    CHECK_RET(sock.Socket());
    CHECK_RET(sock.Bind("127.0.0.1", 8000)); // 本机地址
    CHECK_RET(sock.Listen());
    while(1) {
        TcpSocket clisock;
        if (sock.Accept(&clisock) == false) {
            continue;
        }
        // 接收
        std::string buf;
        clisock.Recv(&buf);
        std::cout << "req:["<< buf << "]" << std::endl;
        // 发送
        std::string first = "HTTP/1.1 302 OK\r\n";
        std::string body = "<html><h1>hello world</h1></html>";
        std::stringstream ss;
        ss << "Content-Length: " << body.size() << "\r\n";
        ss << "Content-Type: text/html;charset=UTF-8" << "\r\n";
        std::string head = ss.str(); 
        std::string blank = "\r\n";
        clisock.Send(first);
        clisock.Send(head);
        clisock.Send(blank);
        clisock.Send(body);
    }
    return 0;
}
