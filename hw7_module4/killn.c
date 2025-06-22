#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <PID> <SIGNAL_NAME>\n", argv[0]);
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
    int signal_num = -1;
    for (int i = 0; i < num_signals; i++) {
        if (strcmp(argv[2], signal_names[i]) == 0) {
            signal_num = signal_numbers[i];
            break;
        }
    }
    if (signal_num == -1) {
        fprintf(stderr, "No such signal: %s\n", argv[2]);
        return 1;
    }
    pid_t pid = atoi(argv[1]);
    if (kill(pid, signal_num)) {
        perror("Failed to kill");
        return 1;
    }
    return 0;
} 