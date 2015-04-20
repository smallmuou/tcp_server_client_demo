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
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>

void sigint_handle(int sig);
int socket_init();
void socket_deinit(int fd);
void socket_process(int clientfd);
void socket_accept(int fd);

unsigned char quit = 0;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Listen Port>\n", argv[0]);
        return -1;
    }

    signal(SIGINT, sigint_handle);

    int listenfd = socket_init(atoi(argv[1]));
    if (listenfd < 0) {
        exit(-1);
    }

    socket_accept(listenfd);

    socket_deinit(listenfd);
    return 0;
}

void sigint_handle(int sig) {
    quit = 1;
}

int socket_init(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("socket create failed.\n");
        goto _error;
    }

    //reuse address
    int opt = SO_REUSEADDR;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 

    //bind
    struct sockaddr_in addr;  
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("socket bind failed.\n");
        goto _error;
    }

    //listen
    if(listen(fd, 10) < 0) {
        printf("socket listen failed.\n");
        goto _error;
    }  

    return fd;
_error:
    socket_deinit(fd);
    return -1;
}

void socket_accept(int fd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen;
    fd_set fds;
    struct timeval timeout = {3, 0};

    while (!quit) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        int ret = select(fd+1, &fds, &fds, NULL, &timeout);
        if (ret < 0) {
            break;
        } else if (ret == 0) {
            continue;
        } else {
            int clientfd = accept(fd, (struct sockaddr*)&client_addr, &addrlen);
            if (clientfd < 0) {
                printf("socket accept failed.\n");
                break;
            }

            printf("Accept: %s:%d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
            socket_process(clientfd);
        }
    }
}

static void* thread_proc(void* arg) {
    int clientfd = (int)arg;
    if (clientfd < 0) return NULL;

    send(clientfd, "Welcome", 8, 0);

    close(clientfd);
    return NULL;
}

void socket_process(int clientfd) {
    pthread_t pid;
    int error = pthread_create(&pid, NULL, thread_proc, (void*)clientfd);
    if (error) {
        printf("pthread create failed:%d\n", error);
    }
}

void socket_deinit(int fd) {
    if (fd >= 0) close(fd);
}
