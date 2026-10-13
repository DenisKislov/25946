#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

// Функция чтения динамической строки произвольной длины
char *read_dynamic_line(void) {
    size_t capacity = 32;
    size_t length = 0;
    char *buffer = malloc(capacity);
    if (!buffer) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        // Игнорируем символ возврата каретки '\r', если терминал его передает
        if (c == '\r') {
            continue;
        }

        if (length + 1 >= capacity) {
            capacity *= 2;
            char *new_buf = realloc(buffer, capacity);
            if (!new_buf) {
                perror("realloc failed");
                free(buffer);
                exit(EXIT_FAILURE);
            }
            buffer = new_buf;
        }
        buffer[length++] = (char)c;
    }

    // Если достигнут конец файла (EOF) и ничего не было введено
    if (c == EOF && length == 0) {
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    return buffer;
}

int main(void) {
    Node *head = NULL;

    printf("Enter strings (type '.' on a new line to stop):\n");
    fflush(stdout);

    while (1) {
        char *line = read_dynamic_line();
        if (!line) {
            break;
        }

        // Проверка условия остановки:
        // Строка либо строго равна ".", либо начинается с точки
        if (strcmp(line, ".") == 0 || line[0] == '.') {
            free(line);
            break;
        }

        Node *new_node = malloc(sizeof(Node));
        if (!new_node) {
            perror("malloc failed for node");
            free(line);
            exit(EXIT_FAILURE);
        }
        new_node->str = line;

        // Вставка в НАЧАЛО списка (для обратного порядка вывода)
        new_node->next = head;
        head = new_node;
    }

    printf("\n--- Resulting list of lines (reversed order) ---\n");
    Node *curr = head;
    while (curr) {
        printf("%s\n", curr->str);
        curr = curr->next;
    }
    fflush(stdout);

    // Освобождение динамической памяти
    curr = head;
    while (curr) {
        Node *temp = curr;
        curr = curr->next;
        free(temp->str);
        free(temp);
    }

    return EXIT_SUCCESS;
}
