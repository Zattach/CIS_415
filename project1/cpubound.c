#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))

volatile int ALRM_seen = 0;

static void onalrm(UNUSED int sig) {
    ALRM_seen++;
}

int main(int argc, char **argv) {
    long long i;
    int minutes = 1;
    char name[128];

/*
 * process environment variable and command line arguments
 */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-minutes") == 0) {
            i++;
            minutes = atoi(argv[i]);
        } else if (strcmp(argv[i], "-name") == 0) {
            i++;
            strcpy(name, argv[i]);
        } else {
            fprintf(stderr, "Illegal flag: `%s'\n", argv[i]);
            exit(1);
        }
    }
/*
 * establish ARLM signal handler
 */
    if (signal(SIGALRM, onalrm) == SIG_ERR) {
        fprintf(stderr, "Can't establish SIGALRM handler\n");
        exit(1);
    }
/*
 * set timer into the future
 */
    (void)alarm((unsigned int) 60 * minutes);
    while (! ALRM_seen) {
        for (i = 0; i < 300000000; i++) {
            ;
        }
    }
    return 0;
}
