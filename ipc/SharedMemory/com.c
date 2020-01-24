#include "com.h"

static int Command(int size, int flag) {
    key_t key = ftok(".", 723);
    if (key < 0) {
        perror("ftok");
        return -1;
    }
    int shmid = shmget(key, size, flag);
    if (shmid < 0) {
        perror("ftok");
        return -1;
    }
    return shmid;
}

int CreateShm(int size) {
    return Command(size, IPC_CREAT | IPC_EXCL | 0666);
}

int GetShm(int size) {
    return Command(size, IPC_CREAT);
}

int CatShm(int shmid, void** addr) {
    *addr = shmat(shmid, NULL, 0);
    if (*addr < (void*)0) {
        perror("shmat");
        return -1;
    }
    return 0;
}

int DtShm(const void* addr) {
    int ret = shmdt(addr);
    if (ret < 0) {
        perror("shmdt");
        return -1;
    }
    return 0;
}

int DestoryShm(int shmid) {
    if(shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl");
        return -1;
    }
    return 0;
}
