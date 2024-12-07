#include "gameLogic.h"
#include <string.h>
#include <stdio.h>

static char input_buffer[50] = ""; // 최대 길이 10
static int string_length = 0;

void set_string_length(int length) {
    string_length = length;
}

int get_string_length() {
    return string_length;
}

const char *get_current_input() {
    return input_buffer;
}
void add_input(const char *current_text, const char *new_char) {
    if (strlen(current_text) >= get_string_length()) {
        reset_input();
        strncat(input_buffer, new_char, 1);
        return;
    }

    for (int i = 0; current_text[i] != '\0'; ++i) {
        if (current_text[i] == new_char[0]) {
            printf("중복된 숫자입니다. (입력값: %s, 중복된 숫자: %c)\n", current_text, new_char[0]);
            return;
        }
    }

    strncat(input_buffer, new_char, 1);
}

int is_game_complete() {
    return strlen(input_buffer) == string_length;
}

void reset_input() {
    input_buffer[0] = '\0';
}
