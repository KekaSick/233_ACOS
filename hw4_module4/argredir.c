#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s infile outfile command [arg1] [arg2] ...\n", argv[0]);
        exit(1);
    }
    
    char *infile = argv[1];
    char *outfile = argv[2];
    char *command = argv[3];
    
    int cmd_argc = argc - 3;
    char **cmd_argv = malloc((cmd_argc + 1) * sizeof(char *));
    if (cmd_argv == NULL) {
        perror("malloc failed");
        exit(1);
    }
    
    for (int i = 0; i < cmd_argc; i++) {
        cmd_argv[i] = argv[i + 3];
    }
    cmd_argv[cmd_argc] = NULL;
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        free(cmd_argv);
        exit(1);
    } else if (pid == 0) {
        int infd = open(infile, O_RDONLY);
        if (infd < 0) {
            perror("Failed to open input file");
            free(cmd_argv);
            exit(1);
        }
        
        int outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfd < 0) {
            perror("Failed to open output file");
            close(infd);
            free(cmd_argv);
            exit(1);
        }
        
        if (dup2(infd, STDIN_FILENO) < 0) {
            perror("Failed to redirect stdin");
            close(infd);
            close(outfd);
            free(cmd_argv);
            exit(1);
        }
        
        if (dup2(outfd, STDOUT_FILENO) < 0) {
            perror("Failed to redirect stdout");
            close(infd);
            close(outfd);
            free(cmd_argv);
            exit(1);
        }
        
        close(infd);
        close(outfd);
        
        execvp(command, cmd_argv);
        perror("execvp failed");
        free(cmd_argv);
        exit(1);
    } else {
        int wstatus;
        pid_t wait_result = waitpid(pid, &wstatus, 0);
        
        if (wait_result < 0) {
            perror("waitpid failed");
            free(cmd_argv);
            exit(1);
        }
        
        if (WIFEXITED(wstatus)) {
            printf("%d received\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("Child terminated by signal %d\n", WTERMSIG(wstatus));
        } else {
            printf("Child terminated abnormally\n");
        }
    }
    
    free(cmd_argv);
    return 0;
} 