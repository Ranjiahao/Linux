#include "com.h"
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int semid = CreateSem(1);
    InitSem(semid, 0, 1);
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        while (1) {
            P(semid, 0);
            printf("B");
            usleep(100000);
            printf("B ");
            fflush(stdout);
            usleep(100000);
            V(semid, 0);
        }
    } else {
        while (1) {
            P(semid, 0);
            printf("A");
            usleep(100000);
            printf("A ");
            fflush(stdout);
            usleep(100000);
            V(semid, 0);
        }
        wait(NULL);
    }
    DestorySem(semid);
    return 0;
}
