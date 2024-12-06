#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PORTNUM 9000
#define MAX_INPUT_LENGTH 10

int sd;
char buf[256];

void get_length() {
    // 정답 길이 read
    if (read(sd, buf, sizeof(buf)) == -1) {
        perror("read");
        exit(1);
    }

    buf[strcspn(buf, "\n")] = '\0'; // 입력 끝의 개행 문자 제거
    printf("Solution length: %c\n", buf[0]);
}

int main(void) {
    struct sockaddr_in server;

    // 서버 소켓 생성
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset((char *)&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORTNUM);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 서버와 연결
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected to server.\n");

    // 정답 길이 read
    if (read(sd, buf, sizeof(buf)) == -1) {
        perror("read");
        exit(1);
    }

    buf[strcspn(buf, "\n")] = '\0'; // 입력 끝의 개행 문자 제거
    printf("Solution length: %s\n", buf);

    while (1) {
        printf("Enter your guess (e.g., 64382): ");
        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = '\0'; // 입력 끝의 개행 문자 제거

        // 서버로 입력 전송
        if (write(sd, buf, strlen(buf)) == -1) {
            perror("write");
            exit(1);
        }

        // 서버로부터 응답 받기
        int bytes_read = read(sd, buf, sizeof(buf));
        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }

        buf[bytes_read] = '\0'; // 응답 데이터 처리
        printf("Server: %s\n", buf);

        // "You win!" 메시지를 받으면 게임 종료
        if (buf[0] == 'Y') {
            get_length();
        }
    }

    close(sd);
    return 0;
}
