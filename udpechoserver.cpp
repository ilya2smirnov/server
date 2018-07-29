#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>

#include "mymath.h"
#include "udpechoserver.h"

UdpEchoServer::UdpEchoServer(const uint16_t p) : PORT(p)
{
    int (UdpEchoServer::*func)(const uint16_t);
    func = &UdpEchoServer::startNewThread;
    servThread = std::thread(func, this,  p);
}

int UdpEchoServer::getPort()
{
    return PORT;
}

void UdpEchoServer::join()
{
    if (servThread.joinable()) servThread.join();
}

UdpEchoServer::~UdpEchoServer()
{
   join();
}

int UdpEchoServer::startNewThread(const uint16_t port)
{
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("could not create socket\n");
        return 1;
    }

    if ((bind(sock, (struct sockaddr *)&server_address,
              sizeof(server_address))) < 0) {
        printf("could not bind socket\n");
        return 1;
    }

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(server_address));
    socklen_t client_address_len = 0;

    while (true) {
        char buffer[MAX_MESSAGE];
        ssize_t len = recvfrom(sock, buffer, sizeof(buffer), 0,
                           (struct sockaddr *)&client_address,
                           &client_address_len);

        if (len > 0) buffer[len] = '\0';
        printf("[UDP] %s : %s\n", inet_ntoa(client_address.sin_addr), buffer);
        fflush(stdout);
        if (len > 0) calc(buffer);

        sendto(sock, buffer, static_cast<size_t>(len), 0, (struct sockaddr *)&client_address,
               sizeof(client_address));
    }
}
