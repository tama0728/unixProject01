#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PORTNUM 9000

int main(void) {
    char buf[256];
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);

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

    printf("Waiting ...\n");

    if (listen(sd, 5)) {
        perror("listen");
        exit(1);
    }

    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
        perror("accept");
        exit(1);
    }

    printf("[Client Connected]\n");

    int c = 0;

    while (1) {
        if (read(ns, buf, sizeof(buf)) == -1) {
            perror("read");
            exit(1);
        }
        if (strcmp(buf, "Q") == 0 || strcmp(buf, "q") == 0) {
            printf("[Client Disconnected]\n");
            break;
        } else if (strcmp(buf, "C") == 0 || strcmp(buf, "c") == 0) {
            printf("[Message Cnt Response (cnt: %d) ]\n", c);
            sprintf(buf, "%d", c);
        } else {
            printf("Received Message: %s\n", buf);
            c++;
        }

        if (write(ns, buf, strlen(buf) + 1) == -1) {
            perror("send");
            exit(1);
        }
    }
    close(ns);
    close(sd);

    return 0;
}
