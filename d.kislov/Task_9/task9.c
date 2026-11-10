#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int wait_for_child = 1;
    const char *path;
    pid_t child;
    int status = 0;

    if (argc == 2) {
        path = argv[1];
    } else if (argc == 3 && strcmp(argv[1], "--no-wait") == 0) {
        wait_for_child = 0;
        path = argv[2];
    } else {
        fprintf(stderr, "Usage: %s [--no-wait] file\n", argv[0]);
        return EXIT_FAILURE;
    }

    child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child == 0) {
        execlp("cat", "cat", "--", path, (char *)NULL);
        perror("cat");
        _exit(127);
    }

    printf("Parent: child started.\n");
    fflush(stdout);

    if (wait_for_child) {
        while (waitpid(child, &status, 0) == -1) {
            if (errno != EINTR) {
                perror("waitpid");
                return EXIT_FAILURE;
            }
        }
    }

    if (printf("Parent: last line.\n") < 0 || fflush(stdout) == EOF) {
        return EXIT_FAILURE;
    }

    if (wait_for_child) {
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        if (WIFSIGNALED(status)) {
            return 128 + WTERMSIG(status);
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
