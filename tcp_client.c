/**
 * license-c.c
 * create by Wenva.
 *
 * MIT licence follows:
 *
 * Copyright (C) 2014 Wenva <lvyexuwenfa100@126.com>

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>  
#include <fcntl.h>
#include <strings.h>

int socket_init();
void socket_deinit();
void socket_connect(int fd, char* ip, int port);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Server IP> <Server Port>\n", argv[0]);
        return -1;
    }

    int sockfd = socket_init();
    socket_connect(sockfd, argv[1], atoi(argv[2]));
    printf("Connected to %s\n", argv[1]);

    char buf[1024] = {0};
    int len = recv(sockfd, buf, sizeof(buf), 0);
    if (len >= 0) {
        buf[len] = '\0';
    }
    printf("RECV: %s\n", buf);

    socket_deinit(sockfd);
    return 0;
}

void socket_connect(int fd, char* ip, int port) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(fd, (struct sockaddr*)&addr, addrlen) < 0) {
        perror("socket connect failed");
        exit(-1);
    }
}

int socket_init() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket create failed.");
        exit(-1);
    }

    return fd;
}

void socket_deinit(int fd) {
    if (fd >= 0) close(fd);
}


