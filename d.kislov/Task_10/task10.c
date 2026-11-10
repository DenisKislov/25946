#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t child;
    int status;
    int code;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [argument ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child == 0) {
        execvp(argv[1], &argv[1]);
        code = errno == ENOENT ? 127 : 126;
        perror(argv[1]);
        _exit(code);
    }

    while (waitpid(child, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
    }

    if (WIFEXITED(status)) {
        code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        code = 128 + WTERMSIG(status);
    } else {
        return EXIT_FAILURE;
    }

    if (printf("Exit code: %d\n", code) < 0 || fflush(stdout) == EOF) {
        return EXIT_FAILURE;
    }
    return code;
}
