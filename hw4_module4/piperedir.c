#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        close(pipefd[0]);
        close(pipefd[1]);
        free(cmd2_argv);
        exit(1);
    } else if (pid == 0) {
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
    } else {
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
    
    return 0;
} 