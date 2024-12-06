#include "gameLogic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <gtk/gtk.h>
#include <ctype.h>

static char input_buffer[11] = ""; // 최대 길이 10
static int string_length = 0;
static char solution[11] = "64382";
static char result[50];
static int gameCount = 0;
static int gameStatus = 0;

void set_string_length(int length) {
    string_length = length;
}

int get_string_length() {
    return string_length;
}

const char *get_current_input() {
    return input_buffer;
}

void set_solution(const char *input) {
    strcpy(solution, input);
    set_string_length(strlen(input));
}

void set_input(const char *input) {
    strcpy(input_buffer, input);
}

int get_game_status() {
    return gameStatus;
}

void clear_game_status() {
    gameStatus = 0;
    gameCount = 0;
}

int validate_input(const char *input) {
    int i;
    for (i = 0; input[i] != '\0'; ++i) {
        if (!isdigit(input[i])) return 0;
    }
    return (i > 0 && i < 10) ? 1 : 0;
}

void add_input(const char *current_text, const char *new_char) {
    if (strlen(current_text) >= get_string_length()) {
//        g_print("문자열 길이를 초과했습니다. (입력값: %s)\n", current_text);
        reset_game();
        strncat(input_buffer, new_char, 1);
//        gtk_editable_set_text(GTK_EDITABLE(entry), new_char); // 입력 창 초기화
        return;
    }

    for (int i = 0; current_text[i] != '\0'; ++i) {
        if (current_text[i] == new_char[0]) {
            printf("중복된 숫자입니다. (입력값: %s, 중복된 숫자: %c)\n", current_text, new_char[0]);
            return;
        }
    }

    strncat(input_buffer, new_char, 1);
//    gtk_editable_set_text(GTK_EDITABLE(entry), input_buffer); // 입력 창 초기화
}

int is_game_complete() {
    return strlen(input_buffer) == string_length;
}

const char* numBaseball() {
    int strike = 0, ball = 0;

    if (strlen(input_buffer) != string_length) {
        return 0;
    }

    for (int i = 0; i < string_length; ++i) {
        for (int j = 0; j < string_length; ++j) {
            if (solution[i] == input_buffer[j]) {
                if (i == j) {
                    strike++;
                } else {
                    ball++;
                }
            }
        }
    }
    sprintf(result, "%d: Strike: %d, Ball: %d\n", gameCount+1, strike, ball);
    gameCount++;
    if (strike == string_length) {
        printf("You win!\n");
        gameStatus = 1;
        return "You win!\n";
    } else if (gameCount >= 9) {
        sprintf(result, "You lose! The answer was %s\n", solution);
        gameStatus = 1;
        return result;
    } else {
        return result;
    }
}

void reset_game() {
    input_buffer[0] = '\0';
}
