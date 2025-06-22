#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s command1 command2 [arg1] [arg2] ...\n", argv[0]);
        exit(1);
    }
    
    char *command1 = argv[1];
    char *command2 = argv[2];
    
    int cmd2_argc = argc - 2;
    char **cmd2_argv = malloc((cmd2_argc + 1) * sizeof(char *));
    if (cmd2_argv == NULL) {
        perror("malloc failed");
        exit(1);
    }
    
    for (int i = 0; i < cmd2_argc; i++) {
        cmd2_argv[i] = argv[i + 2];
    }
    cmd2_argv[cmd2_argc] = NULL;
    
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe failed");
        free(cmd2_argv);
        exit(1);
    }
    
    pid_t pid1 = fork();
    
    if (pid1 < 0) {
        perror("fork failed for command1");
        close(pipefd[0]);
        close(pipefd[1]);
        free(cmd2_argv);
        exit(1);
    } else if (pid1 == 0) {
        close(pipefd[0]);
        
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
            perror("Failed to redirect stdout");
            close(pipefd[1]);
            free(cmd2_argv);
            exit(1);
        }
        
        close(pipefd[1]);
        
        execlp(command1, command1, NULL);
        perror("execlp failed for command1");
        free(cmd2_argv);
        exit(1);
    }
    
    pid_t pid2 = fork();
    
    if (pid2 < 0) {
        perror("fork failed for command2");
        close(pipefd[0]);
        close(pipefd[1]);
        free(cmd2_argv);
        exit(1);
    } else if (pid2 == 0) {
        close(pipefd[1]);
        
        if (dup2(pipefd[0], STDIN_FILENO) < 0) {
            perror("Failed to redirect stdin");
            close(pipefd[0]);
            free(cmd2_argv);
            exit(1);
        }
        
        close(pipefd[0]);
        
        execvp(command2, cmd2_argv);
        perror("execvp failed for command2");
        free(cmd2_argv);
        exit(1);
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    int children_completed = 0;
    
    while (children_completed < 2) {
        int wstatus;
        pid_t wait_result = wait(&wstatus);
        
        if (wait_result < 0) {
            if (errno == ECHILD) {
                break;
            } else {
                perror("wait failed");
                free(cmd2_argv);
                exit(1);
            }
        } else if (wait_result == pid1) {
            if (WIFEXITED(wstatus)) {
                printf("Command1 (%s) completed with status %d\n", command1, WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                printf("Command1 (%s) terminated by signal %d\n", command1, WTERMSIG(wstatus));
            }
            children_completed++;
        } else if (wait_result == pid2) {
            if (WIFEXITED(wstatus)) {
                printf("Command2 (%s) completed with status %d\n", command2, WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                printf("Command2 (%s) terminated by signal %d\n", command2, WTERMSIG(wstatus));
            }
            children_completed++;
        }
    }
    
    free(cmd2_argv);
    return 0;
} 