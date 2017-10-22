#ifndef SOCKETMUSTERPROGRAMM_SERVER_H
#define SOCKETMUSTERPROGRAMM_SERVER_H

#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>

#define UDP_BUFFER_SIZE 1400

int server_upd_init(char *ipAddress, uint16_t port);

int server_main_loop (int socketFd);

ssize_t server_send_message (int socketFd,
                             char *message, size_t messageSize,
                             struct sockaddr *clientAddress,
                             socklen_t clientAddressLength);

#endif //SOCKETMUSTERPROGRAMM_SERVER_H
