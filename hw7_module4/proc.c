#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <timeout>\n", argv[0]);
        return 1;
    }
    int timeout = atoi(argv[1]);
    if (timeout <= 0) {
        fprintf(stderr, "Timeout must be a positive integer\n");
        return 1;
    }
    int i;
    for (i = 0;; i++) {
        sleep(timeout);
        printf("%d: %d\n", getpid(), i);
    }
    return 0;
} 