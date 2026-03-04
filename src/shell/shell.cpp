#include "printf.h"
#include "io.h"

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
    else {
        printf("\nUnknown command: %s\n", argv[0]);
    }
}