#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include "client.h"

#define SERVER_PORT 8080

int main (int argc, char **argv)
{
  if (argc < 2)
  {
    printf ("%s IP_ADDRESSS\nExample: %s 192.168.0.1\n",
            basename(argv[0]),
            basename(argv[0]));

    return EXIT_FAILURE;
  }

  char *ipAddress = argv[1];

  int socketFd = client_udp_init(ipAddress, SERVER_PORT);

  int clientResult = client_main_loop (socketFd);

  if (clientResult == EXIT_SUCCESS)
  {
    printf ("Client finished.\n");
  } else {
    printf ("Error in Client.\n");
  }

  return clientResult;
}
