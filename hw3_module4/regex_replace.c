#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_GROWTH_FACTOR 2

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <regex_pattern> <text> <replacement>\n", argv[0]);
        return 1;
    }

    char *pattern = argv[1];
    char *boring_text = argv[2];
    char *replacement = argv[3];
    
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        char error_msg[256];
        regerror(ret, &regex, error_msg, sizeof(error_msg));
        fprintf(stderr, "Regex compilation error: %s\n", error_msg);
        return 1;
    }

    size_t buffer_size = INITIAL_BUFFER_SIZE;
    char *result_buffer = malloc(buffer_size);
    if (!result_buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        regfree(&regex);
        return 1;
    }

    size_t result_pos = 0;
    size_t text_pos = 0;
    size_t text_len = strlen(boring_text);
    
    regmatch_t match;
    
    while (regexec(&regex, boring_text + text_pos, 1, &match, 0) == 0) {
        size_t match_start = text_pos + match.rm_so;
        size_t match_end = text_pos + match.rm_eo;
        
        size_t text_before_match = match_start - text_pos;
        
        size_t replacement_len = strlen(replacement);
        size_t new_content_len = text_before_match + replacement_len;
        
        while (result_pos + new_content_len >= buffer_size) {
            buffer_size *= BUFFER_GROWTH_FACTOR;
            char *new_buffer = realloc(result_buffer, buffer_size);
            if (!new_buffer) {
                fprintf(stderr, "Memory reallocation failed11\n");
                free(result_buffer);
                regfree(&regex);
                return 1;
            }
            result_buffer = new_buffer;
        }
        
        if (text_before_match > 0) {
            memcpy(result_buffer + result_pos, boring_text + text_pos, text_before_match);
            result_pos += text_before_match;
        }
        
        memcpy(result_buffer + result_pos, replacement, replacement_len);
        result_pos += replacement_len;
        
        text_pos = match_end;
        
        if (text_pos >= text_len) {
            break;
        }
    }
    
    size_t leftover_text = text_len - text_pos;
    if (leftover_text > 0) {
        while (result_pos + leftover_text >= buffer_size) {
            buffer_size *= BUFFER_GROWTH_FACTOR;
            char *new_buffer = realloc(result_buffer, buffer_size);
            if (!new_buffer) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(result_buffer);
                regfree(&regex);
                return 1;
            }
            result_buffer = new_buffer;
        }
        
        memcpy(result_buffer + result_pos, boring_text + text_pos, leftover_text);
        result_pos += leftover_text;
    }
    
    result_buffer[result_pos] = '\0';
    
    printf("%s\n", result_buffer);
    
    free(result_buffer);
    regfree(&regex);
    
    return 0;
} 