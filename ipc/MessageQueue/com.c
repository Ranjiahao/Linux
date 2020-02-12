#include "com.h"

static int Commend(int flags) {
    key_t key = ftok(".", 723);
    if (key < 0) {
        perror("ftok");
        return -1;
    }
    int msgid = msgget(key, flags);
    if (msgid < 0) {
        perror("ftok");
        return -1;
    }
    return msgid;
}

int CreateMsgQueue() {
    return Commend(IPC_CREAT | IPC_EXCL | 0666);
}

int GetMsgQueue() {
    return Commend(IPC_CREAT);
}

int SendMsgQueue(int msgid, int type, char* msg) {
    msgbuf buf;
    buf.mtype = type;
    strcpy(buf.mtext, msg);
    if (msgsnd(msgid, &buf, sizeof(buf.mtext), 0) < 0) {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

int RcvMsgQueue(int msgid, int type, char out[]) {
    msgbuf buf;
    if (msgrcv(msgid, &buf, sizeof(buf.mtext), type, 0) < 0) {
        perror("msgrcv");
        return -1;
    }
    strcpy(out, buf.mtext);
    return 0;
}

int DestoryMsgQueue(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) < 0) {
        perror("msgctl");
        return -1;
    }
    return 0;
}
