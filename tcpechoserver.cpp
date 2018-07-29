#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "mymath.h"
#include "tcpechoserver.h"

TcpEchoServer::TcpEchoServer(const uint16_t p) : PORT(p)
{
    int (TcpEchoServer::*func)(const uint16_t);
    func = &TcpEchoServer::startNewThread;
    servThread = std::thread(func, this,  p);
}

int TcpEchoServer::getPort()
{
    return PORT;
}

void TcpEchoServer::join()
{
    if (servThread.joinable()) servThread.join();
}

TcpEchoServer::~TcpEchoServer()
{
   join();
}

void TcpEchoServer::epoll_ctl_add(int epfd, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl error\n");
        exit(1);
    }
}

void TcpEchoServer::set_sockaddr(struct sockaddr_in *addr)
{
    bzero((char *)addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);
}

int TcpEchoServer::setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}


int TcpEchoServer::startNewThread(const uint16_t port)
{
    int epfd;
    int master_fd;
    socklen_t socklen;
    char buf[MAX_MESSAGE];
    struct sockaddr_in srv_addr;
    struct sockaddr_in cli_addr;
    struct epoll_event events[MAX_EVENTS];

    master_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0; //
    setsockopt(master_fd, SOL_SOCKET, SO_SNDTIMEO, &tv,sizeof(tv));
    int optval = 1;
    setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));

    set_sockaddr(&srv_addr);
    bind(master_fd, (sockaddr *)&srv_addr, sizeof(srv_addr));

    setnonblocking(master_fd);
    listen(master_fd, MAX_CONN);

    epfd = epoll_create(1);
    epoll_ctl_add(epfd, master_fd, EPOLLIN | EPOLLOUT | EPOLLET);

    socklen = sizeof(cli_addr);
    while(true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == master_fd) {
                int new_fd = accept(master_fd, (sockaddr *)&cli_addr, &socklen);
                inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), buf, sizeof(cli_addr));
                printf("[+] connected with %s:%d\n", buf, ntohs(cli_addr.sin_port));
                setnonblocking(new_fd);
                epoll_ctl_add(epfd, new_fd,
                          EPOLLIN | EPOLLET | EPOLLRDHUP |
                          EPOLLHUP);
            } else if (events[i].events & EPOLLIN) {
                while(true) {
                    bzero(buf, sizeof(buf));
                    int n = read(events[i].data.fd, buf, sizeof(buf));
                    if (n <= 0) break;
                    else {
                            struct sockaddr_in addr;
                            socklen_t addr_size = sizeof(struct sockaddr_in);
                            int res = getpeername(events[i].data.fd, (struct sockaddr *)&addr, &addr_size);
                            char * ip = new char[16];
                            strcpy(ip, inet_ntoa(addr.sin_addr));
                            printf("[TCP] %s : %s\n", ip, buf);
                            fflush(stdout);
                            delete [] ip;
                            calc(buf);
                            write(events[i].data.fd, buf, strlen(buf));
                    }
                }
            } else {
                printf("[+] unexpected\n");
            }
            if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                printf("[+] connection closed\n");
                epoll_ctl(epfd, EPOLL_CTL_DEL,
                      events[i].data.fd, nullptr);
                close(events[i].data.fd);
                continue;
            }
        }
    }
}


