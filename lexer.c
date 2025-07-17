#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN 1000
#define MAX_INPUT 10000 // Larger buffer for multi-line input

// Token types
enum TokenType {
    KEYWORD, IDENTIFIER, NUMBER, OPERATOR, DELIMITER, STRING, UNKNOWN
};

// Token structure
typedef struct {
    char *value;
    enum TokenType type;
    int line;
} Token;

Token tokens[MAX_TOKEN];
int token_count = 0;
int current_line = 1;

// Keywords
const char *keywords[] = {"put", "display", "check", "otherwise", "repeat", "range"};
const int num_keywords = 6;

// Helper functions
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

// Lexer function
void lexer(char *input) {
    token_count = 0;
    char *ptr = input;
    char buffer[100];
    int buf_idx = 0;

    while (*ptr && token_count < MAX_TOKEN) {
        if (*ptr == '\n') {
            current_line++;
            ptr++;
            continue;
        }
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }

        if (*ptr == '"') {
            ptr++;
            buf_idx = 0;
            while (*ptr && *ptr != '"') buffer[buf_idx++] = *ptr++;
            if (!*ptr) {
                printf("Error: Unterminated string at line %d\n", current_line);
                return;
            }
            ptr++;
            buffer[buf_idx] = '\0';
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
    printf("Tokens:\n");
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

void free_tokens() {
    for (int i = 0; i < token_count; i++) free(tokens[i].value);
    token_count = 0;
    current_line = 1;
}

int main() {
    char input[MAX_INPUT] = {0}; // Initialize buffer to zeros
    char line[1000];             // Buffer for each line
    printf("Enter code (end with Ctrl+D on Unix or Ctrl+Z then Enter on Windows):\n");

    // Read multiple lines until EOF (Ctrl+Z on Windows)
    while (fgets(line, 1000, stdin) != NULL) {
        // Check for Ctrl+Z (EOF) immediately after Enter
        if (line[0] == 26) break; // Ctrl+Z is ASCII 26
        strncat(input, line, MAX_INPUT - strlen(input) - 1); // Append line to input
    }

    lexer(input);
    print_tokens();
    free_tokens();
    return 0;
}