#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gameLogic.h"

#define PORTNUM 9000
#define MAX_INPUT_LENGTH 10

char buf[256];
int sd, ns;
char solution[11];

void get_solution() {
    // 정답 입력
    printf("Enter the solution: ");
    fgets(solution, sizeof(solution), stdin);
    solution[strcspn(solution, "\n")] = '\0'; // 입력 끝의 개행 문자 제거
    set_solution(solution);

    // client에게 정답 길이 전송
    char length[2];
    sprintf(length, "%ld", strlen(solution));
    write(ns, length, strlen(length));
}

int main(void) {
    struct sockaddr_in sin, cli;
    int clientlen = sizeof(cli);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("bind");
        exit(1);
    }

    printf("Waiting for client...\n");

    if (listen(sd, 5)) {
        perror("listen");
        exit(1);
    }

    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
        perror("accept");
        exit(1);
    }

    printf("[Client Connected]\n");

    get_solution();

    while (1) {
        int bytes_read = read(ns, buf, sizeof(buf));
        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }

        buf[bytes_read] = '\0'; // 받은 데이터를 문자열로 처리

        printf("Received from client: %s\n", buf);

        if (strcmp(buf, "exit") == 0) {
            break;
        }

        // 게임 로직 처리
        set_input(buf);
        const char *result = numBaseball();

        printf("Result: %s\n", result);
        write(ns, result, strlen(result));

        // 게임 종료 조건
        if (get_game_status()) {
            get_solution();
            reset_game();
            clear_game_status();
        }
    }

    close(ns);
    close(sd);

    return 0;
}
