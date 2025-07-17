#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN 1000
#define MAX_INPUT 10000

enum TokenType { KEYWORD, IDENTIFIER, NUMBER, OPERATOR, DELIMITER, STRING, UNKNOWN };

typedef struct {
    char *value;
    enum TokenType type;
    int line;
} Token;

Token tokens[MAX_TOKEN];
int token_count = 0;
int current_line = 1;

const char *keywords[] = {"put", "display", "check", "otherwise", "repeat", "range"};
const int num_keywords = 6;

int is_keyword(char *str) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

int is_identifier(char *str) {
    if (!isalpha(str[0]) && str[0] != '_') return 0;
    for (int i = 1; i < strlen(str); i++) {
        if (!isalnum(str[i]) && str[i] != '_') return 0;
    }
    return 1;
}

int is_number(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

void lexer(char *input) {
    token_count = 0;
    char *ptr = input;
    char buffer[100];
    int buf_idx = 0;

    while (*ptr && token_count < MAX_TOKEN) {
        if (*ptr == '\n') { current_line++; ptr++; continue; }
        if (isspace(*ptr)) { ptr++; continue; }

        if (*ptr == '"') {
            ptr++; buf_idx = 0;
            while (*ptr && *ptr != '"') buffer[buf_idx++] = *ptr++;
            if (!*ptr) { printf("Error: Unterminated string at line %d\n", current_line); return; }
            ptr++; buffer[buf_idx] = '\0';
            tokens[token_count].value = strdup(buffer);
            tokens[token_count].type = STRING;
            tokens[token_count++].line = current_line;
            continue;
        }

        if (strchr("+-*/%=><(){};", *ptr)) {
            buffer[0] = *ptr++;
            buffer[1] = '\0';
            tokens[token_count].value = strdup(buffer);
            tokens[token_count].type = (strchr("+-*/%=>", buffer[0])) ? OPERATOR : DELIMITER;
            tokens[token_count++].line = current_line;
            continue;
        }

        buf_idx = 0;
        while (*ptr && !isspace(*ptr) && !strchr("+-*/%=><(){};", *ptr)) {
            buffer[buf_idx++] = *ptr++;
        }
        if (buf_idx > 0) {
            buffer[buf_idx] = '\0';
            tokens[token_count].value = strdup(buffer);
            if (is_keyword(buffer)) tokens[token_count].type = KEYWORD;
            else if (is_identifier(buffer)) tokens[token_count].type = IDENTIFIER;
            else if (is_number(buffer)) tokens[token_count].type = NUMBER;
            else tokens[token_count].type = UNKNOWN;
            tokens[token_count++].line = current_line;
        }
    }
}

void print_tokens() {
    printf("\nTokens:\n");
    for (int i = 0; i < token_count; i++) {
        char *type_str;
        switch (tokens[i].type) {
            case KEYWORD: type_str = "Keyword"; break;
            case IDENTIFIER: type_str = "Identifier"; break;
            case NUMBER: type_str = "Number"; break;
            case OPERATOR: type_str = "Operator"; break;
            case DELIMITER: type_str = "Delimiter"; break;
            case STRING: type_str = "String"; break;
            case UNKNOWN: type_str = "Unknown"; break;
        }
        printf("%d: %s (%s, Line %d)\n", i+1, tokens[i].value, type_str, tokens[i].line);
    }
}

// Simple parser for syntax check
void parser() {
    printf("\nParsing...\n");
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == KEYWORD && strcmp(tokens[i].value, "put") == 0) {
            if (i+2 < token_count &&
                tokens[i+1].type == IDENTIFIER &&
                tokens[i+2].type == OPERATOR &&
                strcmp(tokens[i+2].value, "=") == 0) {
                i += 2;
                continue;
            } else {
                printf("Syntax Error at line %d near '%s'\n", tokens[i].line, tokens[i].value);
                return;
            }
        }
    }
    printf("Parsing successful.\n");
}

// Simple semantic check
void semantic_analyzer() {
    printf("\nSemantic Analysis...\n");
    int defined = 0;
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == KEYWORD && strcmp(tokens[i].value, "put") == 0) {
            if (i+1 < token_count && tokens[i+1].type == IDENTIFIER) defined = 1;
        }
        if (tokens[i].type == IDENTIFIER && !defined) {
            printf("Semantic Error: Undeclared identifier '%s' at line %d\n", tokens[i].value, tokens[i].line);
            return;
        }
    }
    printf("Semantic analysis passed.\n");
}

// Pseudo code generation
void code_generator() {
    printf("\nGenerated Code:\n");
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == KEYWORD && strcmp(tokens[i].value, "put") == 0) {
            printf("MOV ");
            if (i+3 < token_count) {
                printf("%s, %s\n", tokens[i+1].value, tokens[i+3].value);
                i += 3;
            }
        } else if (tokens[i].type == KEYWORD && strcmp(tokens[i].value, "display") == 0) {
            if (i+1 < token_count)
                printf("PRINT %s\n", tokens[i+1].value);
        }
    }
}

void free_tokens() {
    for (int i = 0; i < token_count; i++) free(tokens[i].value);
    token_count = 0;
    current_line = 1;
}

int main() {
    char input[MAX_INPUT] = {0};
    char line[1000];
    printf("Enter code (end with Ctrl+D/Ctrl+Z):\n");
    while (fgets(line, 1000, stdin) != NULL) {
        if (line[0] == 26) break;
        strncat(input, line, MAX_INPUT - strlen(input) - 1);
    }

    lexer(input);
    print_tokens();
    parser();
    semantic_analyzer();
    code_generator();
    free_tokens();
    return 0;
}
