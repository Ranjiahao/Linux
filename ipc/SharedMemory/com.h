#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int CreateShm(int size);
int GetShm(int size);
int CatShm(int shmid, void** addr);
int DtShm(const void* addr);
int DestoryShm(int shmid);
