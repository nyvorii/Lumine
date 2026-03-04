#ifndef SHELL_H
#define SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

// Usunęliśmy 'const', bo tokenizer wpisuje '\0' w miejsca spacji
void execute_command(char* command);
int tokenize(char* line, char** argv);

#ifdef __cplusplus
}
#endif

#endif