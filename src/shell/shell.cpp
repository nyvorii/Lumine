#include "printf.h"
#include "io.h"
#include "sleep.h"
#include <stddef.h>
#include "uptime.h"

int tokenize(char* line, char** argv) {
    int argc = 0;
    bool in_word = false;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] != ' ' && !in_word) {
            argv[argc++] = &line[i];
            in_word = true;
        } else if (line[i] == ' ') {
            line[i] = '\0';
            in_word = false;
        }
    }
    return argc;
}

extern "C" void execute_command(char* command){
    char* argv[16];
    char argc = tokenize(command, argv);
    if (argc == 0) {
        return; 
    }
    
    if (strcmp(argv[0], "help") == 0) {
        printf("\nAvaiable commands:\n");
        printf("help - shows this message\n");
        printf("clear - clears the screen\n");
        printf("echo - echos a message. Usage: echo [message]\n");
        printf("sleep - pauses execution for a specified number of milliseconds. Usage: sleep [milliseconds]\n");
        printf("uptime - shows how long the system has been running\n");
        printf("info - shows information about the OS\n");
    }
    else if (strcmp(argv[0], "clear") == 0) {
        clear_screen();
    }
    else if (strcmp(argv[0], "echo") == 0) {
        printf("\n");
        for (int i = 1; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
    }
    else if (strcmp(argv[0], "info") == 0) {
        printf("\nLumine OS 0.0.1a\n");
        printf("Created by haru\n");
    }
    else if (strcmp(argv[0], "") == 0) {
        // pusty enter    
        }
    else if (strcmp(argv[0], "sleep") == 0) {
        if (argc < 2) {
            printf("\nUsage: sleep [milliseconds]\n");
            return;
        }
        uint64_t ms = 0;
        for (size_t i = 0; argv[1][i] != '\0'; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            printf("\nInvalid number: %s\n", argv[1]);
            return;
            }
            ms = ms * 10 + (argv[1][i] - '0');
            }
            sleep(ms);
        }
    else if (strcmp(argv[0], "uptime") == 0) {
        uptime();
    }
    else {
        printf("\nUnknown command: %s\n", argv[0]);
    }
}