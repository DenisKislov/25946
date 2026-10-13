#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_LINES 1000
#define TIMEOUT_SEC 5

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

static void print_whole_file(int fd) {
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("lseek to start failed");
        return;
    }
    char buf[512];
    ssize_t n;
    printf("\n--- Timeout (5 sec)! Printing entire file ---\n");
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, n) < 0) {
            perror("write failed");
            break;
        }
    }
    putchar('\n');
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return EXIT_FAILURE;
    }

    LineInfo lines[MAX_LINES];
    int line_count = 0;
    off_t current_offset = 0;
    size_t current_length = 0;
    char buffer[512];
    ssize_t bytes_read;

    lines[line_count].offset = 0;

    // Сканируем файл и строим таблицу смещений
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; ++i) {
            current_length++;
            if (buffer[i] == '\n') {
                lines[line_count].length = current_length;
                line_count++;
                if (line_count >= MAX_LINES) {
                    fprintf(stderr, "Too many lines\n");
                    close(fd);
                    return EXIT_FAILURE;
                }
                current_offset += current_length;
                lines[line_count].offset = current_offset;
                current_length = 0;
            }
        }
    }

    if (current_length > 0) {
        lines[line_count].length = current_length;
        line_count++;
    }

    printf("File indexed: %d lines found. You have %d seconds per query.\n", line_count, TIMEOUT_SEC);

    while (1) {
        printf("Enter line number (1-%d, 0 to quit): ", line_count);
        fflush(stdout);

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        struct timeval tv;
        tv.tv_sec = TIMEOUT_SEC;
        tv.tv_usec = 0;

        int sel = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &tv);
        if (sel == -1) {
            perror("select failed");
            break;
        }
        if (sel == 0) {
            // Сработал тайм-аут 5 секунд
            print_whole_file(fd);
            break;
        }

        int req_line;
        if (scanf("%d", &req_line) != 1) {
            fprintf(stderr, "Invalid input. Exiting.\n");
            break;
        }

        if (req_line == 0) {
            break;
        }

        if (req_line < 1 || req_line > line_count) {
            printf("Error: line number out of range.\n");
            continue;
        }

        int idx = req_line - 1;
        if (lseek(fd, lines[idx].offset, SEEK_SET) == (off_t)-1) {
            perror("lseek failed");
            continue;
        }

        char *line_buf = malloc(lines[idx].length + 1);
        if (!line_buf) {
            perror("malloc failed");
            continue;
        }

        ssize_t r = read(fd, line_buf, lines[idx].length);
        if (r > 0) {
            line_buf[r] = '\0';
            printf("Line %d: %s", req_line, line_buf);
            if (line_buf[r - 1] != '\n') {
                putchar('\n');
            }
        }

        free(line_buf);
    }

    close(fd);
    return EXIT_SUCCESS;
}
