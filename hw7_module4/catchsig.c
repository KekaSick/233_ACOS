#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

void handler(int sig) {
    printf("[Caught: %s]", strsignal(sig));
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <timeout> <SIGNAL1> [SIGNAL2] ...\n", argv[0]);
        return 1;
    }
    int timeout = atoi(argv[1]);
    if (timeout <= 0) {
        fprintf(stderr, "Timeout must be a positive integer\n");
        return 1;
    }
    const char *signal_names[] = {
        "HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "BUS", "FPE",
        "KILL", "USR1", "SEGV", "USR2", "PIPE", "ALRM", "TERM", "STKFLT",
        "CHLD", "CONT", "STOP", "TSTP", "TTIN", "TTOU", "URG", "XCPU",
        "XFSZ", "VTALRM", "PROF", "WINCH", "IO", "PWR", "SYS", "UNUSED"
    };
    const int signal_numbers[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31
    };
    int num_signals = sizeof(signal_names) / sizeof(signal_names[0]);
    for (int i = 2; i < argc; i++) {
        int signal_num = -1;
        for (int j = 0; j < num_signals; j++) {
            if (strcmp(argv[i], signal_names[j]) == 0) {
                signal_num = signal_numbers[j];
                break;
            }
        }
        if (signal_num != -1) {
            if (signal(signal_num, handler) == SIG_ERR) {
                perror("signal");
                return 1;
            }
        } else {
            fprintf(stderr, "Unknown signal: %s\n", argv[i]);
            return 1;
        }
    }
    int i;
    for (i = 0;; i++) {
        sleep(timeout);
        printf("%d: %d\n", getpid(), i);
    }
    return 0;
} 