#include "com.h"
#include <unistd.h>

int main() {
    int shmid = CreateShm(1024);
    char* addr = NULL;
    CatShm(shmid, (void**)&addr);
    for (int i = 0; i < 26; i++) {
        printf("client say: %s\r", addr);
        fflush(stdout);
        sleep(1);
    }
    DtShm(addr);
    DestoryShm(shmid);
    return 0;
}
