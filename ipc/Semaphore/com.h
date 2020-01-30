#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* _buf;
};

int CreateSem(int nums);
int GetSem(int nums);
int InitSem(int semid, int index, int initval);
int P(int semid, int index);
int V(int semid, int index);
int DestorySem(int semid);
