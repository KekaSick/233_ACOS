#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s command infile outfile\n", argv[0]);
        exit(1);
    }
    
    char *command = argv[1];
    char *infile = argv[2];
    char *outfile = argv[3];
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        int infd = open(infile, O_RDONLY);
        if (infd < 0) {
            perror("Failed to open input file");
            exit(1);
        }
        
        int outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfd < 0) {
            perror("Failed to open output file");
            close(infd);
            exit(1);
        }
        
        if (dup2(infd, STDIN_FILENO) < 0) {
            perror("Failed to redirect stdin");
            close(infd);
            close(outfd);
            exit(1);
        }
        
        if (dup2(outfd, STDOUT_FILENO) < 0) {
            perror("Failed to redirect stdout");
            close(infd);
            close(outfd);
            exit(1);
        }
        
        close(infd);
        close(outfd);
        
        execlp(command, command, NULL);
        perror("execlp failed");
        exit(1);
    } else {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        printf("%d received\n", WEXITSTATUS(wstatus));
    }
    
    return 0;
} 