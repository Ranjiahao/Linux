#include "com.h"

static int Command(int nums, int flags ) {
    key_t key = ftok(".", 723);
    if (key < 0) {
        perror("ftok");
        return -1;
    }
    int semid = semget(key, nums, flags);
    if (semid < 0) {
        perror("semget");
        return -1;
    }
    return semid;
}

static int CommandPV(int semid, int index, int val) {
    struct sembuf buf;
    buf.sem_num = index;
    buf.sem_op = val;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) < 0) {
        perror("semop");
        return -1;
    }
    return 0;
}

int CreateSem(int nums) {
    return Command(nums, IPC_CREAT | IPC_EXCL | 0664);
}

int GetSem(int nums) {
    return Command(nums, IPC_CREAT);
}

int InitSem(int semid, int index, int initval) {
    union semun _un;
    _un.val = initval;
    if (semctl(semid, index, SETVAL, _un) < 0) {
        perror("semctl");
        return -1;
    }
    return 0;
}

int P(int semid, int index) {
    return CommandPV(semid, index, -1);
}

int V(int semid, int index) {
    return CommandPV(semid, index, 1);
}

int DestorySem(int semid) {
    if (semctl(semid, 0, IPC_RMID) < 0) {
        perror("semctl");
        return -1;
    }
    return 0;
}
