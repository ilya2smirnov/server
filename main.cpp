#include <iostream>
#include <stdint.h>
#include "udpechoserver.h"
#include "tcpechoserver.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));

    UdpEchoServer udpserver(port);
    std::cout << "UDP server is running" << std::endl;

    TcpEchoServer tcpserver(port);
    std::cout << "TCP server is running" << std::endl;

    return 0;
}
