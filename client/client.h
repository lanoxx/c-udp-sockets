#ifndef SOCKETMUSTERPROGRAMM_CLIENT_H
#define SOCKETMUSTERPROGRAMM_CLIENT_H

#include <stdint.h>

#define UDP_BUFFER_SIZE 1400

int client_udp_init(char *serverIpAddress, uint16_t port);

int client_main_loop (int serverSocketFd);

#endif //SOCKETMUSTERPROGRAMM_CLIENT_H
