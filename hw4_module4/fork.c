#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;
    int status1, status2;
    
    printf("Hello from Parent (PID: %d)\n", getpid());
    
    pid1 = fork();
    
    if (pid1 < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid1 == 0) {
        printf("Hello from First Child (PID: %d, Parent PID: %d)\n", getpid(), getppid());
        
        pid2 = fork();
        
        if (pid2 < 0) {
            perror("Fork failed in first child");
            exit(1);
        } else if (pid2 == 0) {
            printf("Hello from Second Child (PID: %d, Parent PID: %d)\n", getpid(), getppid());
            exit(0);
        } else {
            waitpid(pid2, &status2, 0);
            printf("First Child: Second child completed with status %d\n", WEXITSTATUS(status2));
            exit(0);
        }
    } else {
        waitpid(pid1, &status1, 0);
        printf("Parent: First child completed with status %d\n", WEXITSTATUS(status1));
    }
    
    return 0;
} 