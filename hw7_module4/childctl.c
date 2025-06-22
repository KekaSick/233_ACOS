#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static int quit_count = 0;
static int timeout;
static int quit_signal;

void handler(int sig) {
    printf("[Caught: %s]", strsignal(sig));
    fflush(stdout);
    if (sig == quit_signal) {
        quit_count++;
        printf(" (quit count: %d/3)", quit_count);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <timeout> <signalQ> <signal1> [signal2] ...\n", argv[0]);
        return 1;
    }
    timeout = atoi(argv[1]);
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
    quit_signal = -1;
    for (int i = 0; i < num_signals; i++) {
        if (strcmp(argv[2], signal_names[i]) == 0) {
            quit_signal = signal_numbers[i];
            break;
        }
    }
    if (quit_signal == -1) {
        fprintf(stderr, "Unknown quit signal: %s\n", argv[2]);
        return 1;
    }
    for (int i = 3; i < argc; i++) {
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
    if (signal(quit_signal, handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }
    int stat;
    pid_t pid;
    if ((pid = fork()) == 0) {
        while(1) {
            sleep(1);
        }
    } else if (pid > 0) {
        printf("Forking a child: %d\n", pid);
        int counter = 0;
        while (quit_count < 3) {
            sleep(timeout);
            printf("Parent message %d\n", counter++);
            fflush(stdout);
        }
        printf("Exiting after 3 quit signals\n");
        if (kill(pid, SIGTERM) == -1) {
            perror("kill");
            return 1;
        }
        if (wait(&stat) == -1) {
            perror("wait");
            return 1;
        }
        printf("And finally…\n");
        if (WIFSIGNALED(stat)) {
            psignal(WTERMSIG(stat), "Terminated:");
        }
        printf("Exit status: %d\n", stat);
    } else {
        perror("fork");
        return 1;
    }
    return 0;
} 