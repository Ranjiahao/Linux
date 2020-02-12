#include "com.h"
#include <unistd.h>

int main() {
    int msgid = CreateMsgQueue();
    char buf[1024] = { 0 };
    while (1) {
        RcvMsgQueue(msgid, CLIENT_TYPE, buf);
        printf("client say: %s\n", buf);
        printf("please enter: ");
        fflush(stdout);
        ssize_t s = read(0, buf, sizeof(buf));
        buf[s - 1] = 0;
        SendMsgQueue(msgid, SERVER_TYPE, buf);
    }
    DestoryMsgQueue(msgid);
    return 0;
}
