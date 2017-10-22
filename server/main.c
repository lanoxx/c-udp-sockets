#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "server.h"

#define SERVER_PORT 8080

int main (int argc, char **argv)
{
  char *serverIpAddress;

  if (argc < 2)
  {
    printf("Listening on all interfaces.");
    serverIpAddress = NULL;
  } else
  {
    serverIpAddress = argv[1];
  }

  printf ("Starting Server\n");

  int socketFd = server_upd_init (serverIpAddress,
                                  SERVER_PORT);

  if (socketFd < 0)
  {
    printf("Unable to start server.\n");
    return EXIT_FAILURE;
  }

  char *ipAddress = serverIpAddress ? serverIpAddress : "ALL INTERFACES";
  printf("Server listening on ip: %s and port: %d\n", ipAddress, SERVER_PORT);

  //run main server loop
  int mainLoopResult = server_main_loop (socketFd);

  printf("Server terminated.\n");

  return mainLoopResult;
}
