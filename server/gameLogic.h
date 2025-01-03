#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

// 문자열 길이 설정 및 가져오기
void set_string_length(int length);
void set_solution(const char *input);
void set_input(const char *input);
int get_game_status();
void clear_game_status();

// 입력 검증 및 추가
int validate_input(const char *input);

// 게임 상태
const char* numBaseball();
void reset_game();

#endif // GAME_LOGIC_H
