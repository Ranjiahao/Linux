#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CMD 1024
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

static char command[MAX_CMD];
static char* pipe_command[32];

// 获取命令行
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

// 解析命令并返回argv
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

// 处理命令中的重定向
int do_redirect(char* buff) {
    char* ptr = buff;
    int redirect_type = -1;
    char* file = NULL;
    int fd = -1;
    while (*ptr != '\0') {
        if (*ptr == '>') {
            *ptr++ = '\0';
            redirect_type++;
            if (*ptr == '>') {
                *ptr++ = '\0';
                redirect_type++;
            }
            while (isspace(*ptr)) {
                ptr++;
            }
            file = ptr;
            while ((!isspace(*ptr)) && *ptr != '\0') {
                ptr++;
            }
            *ptr = '\0';
            if (redirect_type == 0) {
                fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
            } else {
                fd = open(file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
            }
            dup2(fd, 1);
        }
        ptr++;
    }
    return 0;
}

// 分割多个命令到pipe_command中并返回管道个数
int do_command(char* buff) {
    int pipe_num = 0;
    char* ptr = buff;
    pipe_command[0] = ptr;
    while (*ptr != '\0') {
        if (*ptr == '|' ) {
            pipe_num++;
            *ptr++ = '\0';
            pipe_command[pipe_num] = ptr;
        }
        ptr++;
    }
    return pipe_num;
}

// 建立子进程，执行命令
int do_pipe(int pipe_num) {
    int(* const pipefd)[2] = (int(*)[2])calloc(pipe_num * 2, sizeof(int));
    if (pipefd == NULL) {
        perror("calloc");
        return -1;
    }
    for (int i = 0; i < pipe_num; i++) {
        pipe(pipefd[i]);
    }
    for (int i = 0; i <= pipe_num; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        } else if (pid == 0) {
            do_redirect(pipe_command[i]);
            char** argv = do_parse(pipe_command[i]);
            if (strcmp(argv[0], "cd") == 0) {
                chdir(argv[1]);
                break;
            }
            if (i != 0) {
                close(pipefd[i - 1][1]);
                dup2(pipefd[i - 1][0], 0);
            }
            if (i != pipe_num) {
                close(pipefd[i][0]);
                dup2(pipefd[i][1], 1);
            }
            execvp(argv[0], argv);
        } else {
            if (i != 0) {
                close(pipefd[i - 1][0]);
                close(pipefd[i - 1][1]);
            }
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}

int main() {
    while (1) {
        if (do_face(command) < 0) {
            continue;
        }
        do_pipe(do_command(command));
    }
    return 0;
}
