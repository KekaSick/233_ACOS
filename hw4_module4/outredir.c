#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s command output_filename\n", argv[0]);
        exit(1);
    }
    
    char *command = argv[1];
    char *output_filename = argv[2];
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        int fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Failed to open output file");
            exit(1);
        }
        
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("Failed to redirect stdout");
            close(fd);
            exit(1);
        }
        
        close(fd);
        
        execlp(command, command, NULL);
        perror("execlp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("Command completed with status %d\n", WEXITSTATUS(status));
    }
    
    return 0;
} 