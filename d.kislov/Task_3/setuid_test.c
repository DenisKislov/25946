#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void print_uids(const char *msg) {
    printf("[%s]\n", msg);
    printf("  Real UID (getuid):      %ld\n", (long)getuid());
    printf("  Effective UID (geteuid): %ld\n", (long)geteuid());
}

void try_open_file(const char *filename) {
    printf("Trying to open '%s'...\n", filename);
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("  fopen failed");
    } else {
        printf("  Success: file opened successfully!\n");
        fclose(f);
    }
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "data.txt";

    // 1. Состояние до сброса привилегий
    print_uids("Initial state");
    try_open_file(filename);

    printf("\n--- Calling setuid(getuid()) ---\n\n");

    // 2. Сброс эффективного UID к значению реального
    if (setuid(getuid()) == -1) {
        perror("setuid failed");
        return EXIT_FAILURE;
    }

    // 3. Состояние после сброса привилегий
    print_uids("After setuid(getuid())");
    try_open_file(filename);

    return EXIT_SUCCESS;
}
