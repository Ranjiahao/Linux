//1. 获取命令行
//2. 解析命令行
//3. 建立一个子进程（fork）
//4. 替换子进程（execvp）
//5. 父进程等待子进程退出（wait）
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX_CMD 1024

int do_face(char* buff) {
    memset(buff, 0, MAX_CMD);
    printf("[minishell]$ ");
    fflush(stdout);
    // %[^\n]  获取字符串直到遇到\n
    // %*c     将一个字符从缓冲区取出
    if (scanf("%[^\n]%*c", buff) == 0) {
        getchar();
        return -1;
    }
    // fgets(buff, MAX_CMD, stdin);
    return 0;
}

char** do_parse(char* buff) {
    int argc = 0;
    static char* argv[32];
    char* ptr = buff;
    while (*ptr != '\0') {
        if (!isspace(*ptr)) {
            argv[argc++] = ptr;
            while ((!isspace(*ptr)) && (*ptr) != '\0') {
                ptr++;
            }
        } else {
            while (isspace(*ptr)) {
                *ptr = '\0';
                ptr++;
            }
        }
    }
    argc[argv] = NULL;
    return argv;
}

int do_exec(char* buff) {
    char** argv = NULL;
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
    } else if (pid == 0) {
        argv = do_parse(buff);
        if (argv[0] == NULL) {
            exit(-1);
        }
        execvp(argv[0], argv);
    } else {
        waitpid(pid, NULL, 0);
    }
    return 0;
}

int main() {
    char command[MAX_CMD];
    while (1) {
        if (do_face(command) == 0) {
            do_exec(command);
        }
    }
    return 0;
}
