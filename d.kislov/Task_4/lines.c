#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

// Функция для чтения строки произвольной длины символ за символом
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

    if (c == EOF && length == 0) {
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    return buffer;
}

int main(void) {
    Node *head = NULL;
    Node *tail = NULL;

    printf("Enter strings (start a line with '.' to stop):\n");

    while (1) {
        char *line = read_dynamic_line();
        if (!line) {
            break;
        }

        // Условие остановки: строка начинается с точки
        if (line[0] == '.') {
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
        new_node->next = NULL;

        if (!head) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    printf("\n--- Resulting list of lines ---\n");
    Node *curr = head;
    while (curr) {
        printf("%s\n", curr->str);
        curr = curr->next;
    }

    // Освобождение памяти
    curr = head;
    while (curr) {
        Node *temp = curr;
        curr = curr->next;
        free(temp->str);
        free(temp);
    }

    return EXIT_SUCCESS;
}
