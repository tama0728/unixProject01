#include "gameLogic.h"
#include <string.h>
#include <stdlib.h>
#include <glib.h>

static int string_length = 0;
static char input_buffer[11] = ""; // 최대 길이 10

void set_string_length(int length) {
    string_length = length;
}

int get_string_length() {
    return string_length;
}

int validate_input(const char *input) {
    for (int i = 0; input[i] != '\0'; ++i) {
        if (!g_ascii_isdigit(input[i])) return 0;
    }
    int length = atoi(input);
    return (length > 0 && length < 10) ? length : 0;
}

int add_input(const char *current_text, const char *new_char) {
    if (strlen(current_text) >= string_length || strchr(current_text, new_char[0])) {
        return 0; // 길이 초과 또는 중복
    }

    strncat(input_buffer, new_char, 1);
    return 1;
}

const char *get_current_input() {
    return input_buffer;
}

int is_game_complete() {
    return strlen(input_buffer) == string_length;
}

void reset_game() {
    input_buffer[0] = '\0';
}
