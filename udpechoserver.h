#ifndef UDPECHOSERVER_H
#define UDPECHOSERVER_H

#include <stdint.h>
#include <thread>

class UdpEchoServer
{
public:
    static constexpr int MAX_MESSAGE = 65536;

    UdpEchoServer(uint16_t);
    ~UdpEchoServer();
    int getPort();

private:
    const uint16_t PORT;
    std::thread servThread;

    UdpEchoServer(UdpEchoServer &);
    UdpEchoServer();
    UdpEchoServer & operator= (UdpEchoServer &);
    int startNewThread(uint16_t);
    void join();
};

#endif // UDPECHOSERVER_H
