#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define INITIAL_CAPACITY 16

typedef struct {
    long offset;   // Позиция начала строки в файле
    int length;    // Длина строки в байтах
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

    int capacity = INITIAL_CAPACITY;
    int num_lines = 0;
    LineInfo *lines = malloc(capacity * sizeof(LineInfo));
    if (!lines) {
        perror("malloc failed");
        close(fd);
        return EXIT_FAILURE;
    }

    char ch;
    long line_start = 0;
    int current_len = 0;

    // Индексация файла с динамическим увеличением таблицы через realloc
    while (read(fd, &ch, 1) > 0) {
        current_len++;
        if (ch == '\n') {
            if (num_lines >= capacity) {
                capacity *= 2;
                LineInfo *tmp = realloc(lines, capacity * sizeof(LineInfo));
                if (!tmp) {
                    perror("realloc failed");
                    free(lines);
                    close(fd);
                    return EXIT_FAILURE;
                }
                lines = tmp;
            }

            lines[num_lines].offset = line_start;
            lines[num_lines].length = current_len;
            num_lines++;

            // Подсказка из методички: текущая позиция через lseek(fd, 0L, 1)
            line_start = (long)lseek(fd, 0L, SEEK_CUR);
            current_len = 0;
        }
    }

    // Обработка последней строки без завершающего '\n'
    if (current_len > 0) {
        if (num_lines >= capacity) {
            capacity += 1;
            LineInfo *tmp = realloc(lines, capacity * sizeof(LineInfo));
            if (!tmp) {
                perror("realloc failed");
                free(lines);
                close(fd);
                return EXIT_FAILURE;
            }
            lines = tmp;
        }
        lines[num_lines].offset = line_start;
        lines[num_lines].length = current_len;
        num_lines++;
    }

    // Отладочный вывод таблицы строк строго по ТЗ
    printf("--- Debug: Line Table ---\n");
    for (int i = 0; i < num_lines; ++i) {
        printf("Line %d: Offset = %ld, Length = %d\n", i + 1, lines[i].offset, lines[i].length);
    }
    printf("-------------------------\n");

    // Интерактивный запрос номеров строк
    while (1) {
        printf("Enter line number (0 to quit): ");
        fflush(stdout);

        int req_line;
        if (scanf("%d", &req_line) != 1) {
            fprintf(stderr, "Invalid input. Exiting.\n");
            break;
        }

        if (req_line == 0) {
            break;
        }

        if (req_line < 1 || req_line > num_lines) {
            printf("Error: line number out of range (1-%d).\n", num_lines);
            continue;
        }

        int idx = req_line - 1;

        if (lseek(fd, (off_t)lines[idx].offset, SEEK_SET) == (off_t)-1) {
            perror("lseek failed");
            continue;
        }

        char *line_buf = malloc((size_t)lines[idx].length + 1);
        if (!line_buf) {
            perror("malloc failed");
            continue;
        }

        ssize_t r = read(fd, line_buf, (size_t)lines[idx].length);
        if (r > 0) {
            line_buf[r] = '\0';
            printf("%s", line_buf);
            if (line_buf[r - 1] != '\n') {
                putchar('\n');
            }
        }
        free(line_buf);
    }

    free(lines);
    close(fd);
    return EXIT_SUCCESS;
}
