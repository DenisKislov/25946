#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

extern char **environ;

typedef struct {
    char opt;
    char *arg;
} Option;

void print_id(void) {
    printf("Real UID: %ld, Effective UID: %ld\n", (long)getuid(), (long)geteuid());
    printf("Real GID: %ld, Effective GID: %ld\n", (long)getgid(), (long)getegid());
}

void set_group_leader(void) {
    if (setpgid(0, 0) == -1) {
        perror("setpgid failed");
    } else {
        printf("Process became group leader. New PGID: %ld\n", (long)getpgrp());
    }
}

void print_pids(void) {
    printf("PID: %ld, PPID: %ld, PGID: %ld\n", 
           (long)getpid(), (long)getppid(), (long)getpgrp());
}

void print_ulimit(void) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
        perror("getrlimit(RLIMIT_NOFILE) failed");
    } else {
        printf("ulimit (max open files): soft = %ld, hard = %ld\n", 
               (long)rl.rlim_cur, (long)rl.rlim_max);
    }
}

void set_new_ulimit(const char *val_str) {
    char *endptr;
    errno = 0;
    long val = strtol(val_str, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val < 0) {
        fprintf(stderr, "Error: invalid ulimit value '%s'\n", val_str);
        return;
    }

    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
        perror("getrlimit failed");
        return;
    }
    rl.rlim_cur = (rlim_t)val;
    if (setrlimit(RLIMIT_NOFILE, &rl) == -1) {
        perror("setrlimit(RLIMIT_NOFILE) failed");
    } else {
        printf("ulimit successfully set to %ld\n", val);
    }
}

void print_core_size(void) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("getrlimit(RLIMIT_CORE) failed");
    } else {
        printf("Core file size limit (bytes): soft = %ld, hard = %ld\n", 
               (long)rl.rlim_cur, (long)rl.rlim_max);
    }
}

void set_core_size(const char *val_str) {
    char *endptr;
    errno = 0;
    long val = strtol(val_str, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val < 0) {
        fprintf(stderr, "Error: invalid core size value '%s'\n", val_str);
        return;
    }

    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("getrlimit failed");
        return;
    }
    rl.rlim_cur = (rlim_t)val;
    if (setrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("setrlimit(RLIMIT_CORE) failed");
    } else {
        printf("Core file size successfully set to %ld bytes\n", val);
    }
}

void print_cwd(void) {
    char buf[PATH_MAX];
    if (getcwd(buf, sizeof(buf)) != NULL) {
        printf("Current directory: %s\n", buf);
    } else {
        perror("getcwd failed");
    }
}

void print_env(void) {
    printf("--- Environment Variables ---\n");
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
    printf("-----------------------------\n");
}

void set_env_var(const char *val_str) {
    if (strchr(val_str, '=') == NULL) {
        fprintf(stderr, "Error: -V argument must be in NAME=VALUE format (got '%s')\n", val_str);
        return;
    }
    char *env_str = strdup(val_str);
    if (!env_str) {
        perror("strdup failed");
        return;
    }
    if (putenv(env_str) != 0) {
        perror("putenv failed");
        free(env_str);
    } else {
        printf("Environment updated: %s\n", val_str);
    }
}

int main(int argc, char *argv[]) {
    char *optstring = "ispuU:cC:dvV:";
    int c;

    Option *opts = malloc(argc * sizeof(Option));
    if (!opts) {
        perror("malloc failed");
        return 1;
    }
    int count = 0;

    while ((c = getopt(argc, argv, optstring)) != -1) {
        if (c == '?') {
            fprintf(stderr, "Invalid option encountered: -%c\n", optopt);
            continue;
        }
        opts[count].opt = (char)c;
        opts[count].arg = optarg ? strdup(optarg) : NULL;
        count++;
    }

    for (int i = count - 1; i >= 0; i--) {
        switch (opts[i].opt) {
            case 'i': print_id(); break;
            case 's': set_group_leader(); break;
            case 'p': print_pids(); break;
            case 'u': print_ulimit(); break;
            case 'U': set_new_ulimit(opts[i].arg); break;
            case 'c': print_core_size(); break;
            case 'C': set_core_size(opts[i].arg); break;
            case 'd': print_cwd(); break;
            case 'v': print_env(); break;
            case 'V': set_env_var(opts[i].arg); break;
        }
        if (opts[i].arg) {
            free(opts[i].arg);
        }
    }

    free(opts);
    return 0;
}
