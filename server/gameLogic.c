#include "gameLogic.h"
#include <string.h>
#include <stdio.h>
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
        // 중복된 숫자가 있는지 검사
        for (int j = i+1; input[j] != '\0'; ++j) {
            if (input[i] == input[j]) {
                return 0;
            }
        }
    }
    return (i > 0 && i < 10) ? 1 : 0;
}

const char* numBaseball() {
    int strike = 0, ball = 0;

    if (strlen(input_buffer) != string_length) {
        return 0;
    }
    // 스트라이크와 볼 계산
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
    // 스트라이크가 정답과 길이가 같으면 승리
    if (strike == string_length) {
        printf("You win!\n");
        gameStatus = 1;
        return "You win!\n";
    }
    // 9번째 시도까지 했는데도 승리하지 못하면 패배
    else if (gameCount >= 9) {
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
