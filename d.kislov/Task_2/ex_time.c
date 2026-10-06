#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main(void) {
    time_t now;
    struct tm *sp;

    // 1. Получаем текущее календарное время в секундах с Epoch (1 января 1970)
    (void) time(&now);

    // 2. Печатаем локальное время системы по умолчанию
    printf("Default time:    %s", ctime(&now));

    // 3. Устанавливаем часовой пояс Калифорнии (Pacific Standard / Daylight Time)
    // putenv помещает указатель на строку напрямую в окружение процесса
    if (putenv("TZ=PST8PDT") != 0) {
        perror("putenv failed");
        return EXIT_FAILURE;
    }

    // 4. Инициализируем данные о часовом поясе
    tzset();

    // 5. Преобразуем время с учётом нового значения TZ
    sp = localtime(&now);
    if (sp == NULL) {
        perror("localtime failed");
        return EXIT_FAILURE;
    }

    // 6. Форматированный вывод времени в Калифорнии
    printf("California time: %d/%d/%02d %d:%02d:%02d %s\n",
           sp->tm_mon + 1,        // tm_mon считается от 0 до 11
           sp->tm_mday,           // день месяца (1-31)
           sp->tm_year % 100,     // две последние цифры года (tm_year считается с 1900)
           sp->tm_hour,           // часы (0-23)
           sp->tm_min,            // минуты (0-59)
           sp->tm_sec,            // секунды (0-59)
           tzname[sp->tm_isdst]); // название пояса (PST или PDT)

    return EXIT_SUCCESS;
}
