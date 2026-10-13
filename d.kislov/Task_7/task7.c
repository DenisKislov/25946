#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINES 1000

typedef struct {
    size_t offset;
    size_t length;
} LineInfo;

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

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat failed");
        close(fd);
        return EXIT_FAILURE;
    }

    if (sb.st_size == 0) {
        printf("File is empty.\n");
        close(fd);
        return EXIT_SUCCESS;
    }

    // Отображаем файл целиком в виртуальную память процесса
    char *map = mmap(NULL, (size_t)sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // Дескриптор можно закрыть сразу: маппинг страниц остаётся активным
    close(fd);

    LineInfo lines[MAX_LINES];
    int line_count = 0;
    size_t line_start = 0;

    lines[line_count].offset = 0;

    // Индексируем строки напрямую по указателю в памяти
    for (size_t i = 0; i < (size_t)sb.st_size; ++i) {
        if (map[i] == '\n') {
            lines[line_count].length = (i - line_start) + 1;
            line_count++;
            if (line_count >= MAX_LINES) {
                fprintf(stderr, "Too many lines (max %d)\n", MAX_LINES);
                break;
            }
            line_start = i + 1;
            lines[line_count].offset = line_start;
        }
    }

    if (line_start < (size_t)sb.st_size && line_count < MAX_LINES) {
        lines[line_count].length = (size_t)sb.st_size - line_start;
        line_count++;
    }

    printf("File mapped: %d lines indexed.\n", line_count);

    while (1) {
        printf("Enter line number (1-%d, 0 to quit): ", line_count);
        fflush(stdout);

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
        // Печать фрагмента памяти заданной длины без lseek и копирования буфера
        printf("Line %d: %.*s", req_line, (int)lines[idx].length, map + lines[idx].offset);
        if (map[lines[idx].offset + lines[idx].length - 1] != '\n') {
            putchar('\n');
        }
    }

    if (munmap(map, (size_t)sb.st_size) == -1) {
        perror("munmap failed");
    }

    return EXIT_SUCCESS;
}
