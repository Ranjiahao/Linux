#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define CLIENT_TYPE 1
#define SERVER_TYPE 2

typedef struct msgbuf {
    long mtype;
    char mtext[1024];
} msgbuf;

int CreateMsgQueue();
int GetMsgQueue();
int SendMsgQueue(int msgid, int type, char* msg);
int RcvMsgQueue(int msgid, int type, char out[]);
int DestoryMsgQueue(int msgid);
