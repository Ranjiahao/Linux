#include "com.h"
#include <unistd.h>

int main() {
    int msgid = GetMsgQueue();
    char buf[1024] = { 0 };
    while (1) {
        printf("please enter: ");
        fflush(stdout);
        ssize_t s = read(0, buf, sizeof(buf));
        buf[s - 1] = 0;
        SendMsgQueue(msgid, CLIENT_TYPE, buf);
        RcvMsgQueue(msgid, SERVER_TYPE, buf);
        printf("server say: %s\n", buf);
    }
    return 0;
}
