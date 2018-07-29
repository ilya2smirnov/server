#ifndef TCPECHOSERVER_H
#define TCPECHOSERVER_H

#include <thread>
#include <netinet/in.h>
#include <stdint.h>

class TcpEchoServer
{
public:
    static constexpr int MAX_MESSAGE = 65536;
    static constexpr int MAX_CONN = 16;
    static constexpr int MAX_EVENTS = 32;

    TcpEchoServer(uint16_t);
    ~TcpEchoServer();
    int getPort();

private:
    const uint16_t PORT;
    std::thread servThread;

    TcpEchoServer(TcpEchoServer &);
    TcpEchoServer();
    TcpEchoServer & operator= (TcpEchoServer &);

    int startNewThread(uint16_t);
    void join();
    int setnonblocking(int);
    void set_sockaddr(sockaddr_in *);
    void epoll_ctl_add(int, int, uint32_t);
};

#endif // TCPECHOSERVER_H
