#include "com.h"
#include <unistd.h>

int main() {
    int shmid = GetShm(1024);
    char* addr = NULL;
    CatShm(shmid, (void**)&addr);
    for (int i = 0; i < 26; i++) {
        addr[i] = 'A' + i;
        addr[i + 1] = '\0';
        sleep(1);
    }
    DtShm(addr);
    return 0;
}
