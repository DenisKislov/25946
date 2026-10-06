#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

extern char *tzname[];

int main(int argc, char *argv[]) {
    time_t now;
    struct tm *sp;

    // По умолчанию Калифорния, либо город/пояс, переданный аргументом
    const char *tz_val = (argc > 1) ? argv[1] : "PST8PDT";

    // Выделяем память в куче под строку окружения "TZ=...",
    // так как putenv требует, чтобы строка существовала всё время работы
    size_t env_len = strlen(tz_val) + 4; // "TZ=" + значение + '\0'
    char *tz_env = malloc(env_len);
    if (!tz_env) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }
    snprintf(tz_env, env_len, "TZ=%s", tz_val);

    // 1. Текущее системное время
    (void) time(&now);
    printf("Default time:    %s", ctime(&now));

    // 2. Установка нового TZ
    if (putenv(tz_env) != 0) {
        perror("putenv failed");
        free(tz_env);
        return EXIT_FAILURE;
    }

    // 3. Обновление системных таблиц часовых поясов
    tzset();

    // 4. Локализация времени под новый пояс
    sp = localtime(&now);
    if (sp == NULL) {
        perror("localtime failed");
        return EXIT_FAILURE;
    }

    // 5. Вывод времени для запрошенного города
    printf("Time in %-8s %d/%d/%02d %d:%02d:%02d %s\n",
           tz_val,
           sp->tm_mon + 1,
           sp->tm_mday,
           sp->tm_year % 100,
           sp->tm_hour,
           sp->tm_min,
           sp->tm_sec,
           tzname[sp->tm_isdst]);

    return EXIT_SUCCESS;
}
