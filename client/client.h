//
// Created by DongYun-Kim on 12/7/24.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "gameLogic.h"

void add_log(const char *input, const char *result);

#endif //CLIENT_CLIENT_H
