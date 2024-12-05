#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <glib.h>

static char input_buffer[11] = ""; // 최대 길이 10

// 문자열 길이 설정 및 가져오기
void set_string_length(int length);
int get_string_length();

// 입력 검증 및 추가
int validate_input(const char *input);
void add_input(const char *current_text, const char *new_char, gpointer entry);
const char *get_current_input();

// 게임 상태
int is_game_complete();
const char* numBaseball();
void reset_game();

#endif // GAME_LOGIC_H
