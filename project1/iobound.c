#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define UNUSED __attribute__((unused))

volatile int ALRM_seen = 0;

static void onalrm(UNUSED int sig) {
    ALRM_seen++;
}

int main(int argc, char **argv) {
    int i, j, minutes = 1, number = 6000000;
    char b[1000];
    int fd = open("/dev/null", O_WRONLY);
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
 * establish SIGALRM handler
 */
    if (signal(SIGALRM, onalrm) == SIG_ERR) {
        fprintf(stderr, "Unable to establish SIGALRM handler\n");
        exit(1);
    }
/*
 * set alarm in the future
 */
    (void) alarm((unsigned int) 60 * minutes);
    while (! ALRM_seen) {
        for (j = 0; j < number; j++)
             (void) write(fd, b, sizeof(b));
    }
    return 0;
}
