#include "server.h"

#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lorem.h"

/*
 * ipAddress: a string containing the ipv4 address to bind the server socket.
 *            Pass NULL to bind to any address.
 *
 * port: The port on which the server socket will listen.
 */
int server_upd_init(char *ipAddress, uint16_t port)
{
  int socketFd = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in socketAddress;
  socketAddress.sin_port = port;
  socketAddress.sin_family = AF_INET;

  if (ipAddress != NULL) {
    int ipConversionResult = inet_aton (ipAddress, &socketAddress.sin_addr);

    if (ipConversionResult != 1)
    {
      printf("Invalid IP Address format. Expected valid IPv4 address (e.g. 192.168.0.1).\n");
      return -1;
    }
  } else {
    socketAddress.sin_addr.s_addr = INADDR_ANY;
  }

  socklen_t socketAddressLength = sizeof(socketAddress);

  int bindResult = bind(socketFd,
                        (struct sockaddr *) &socketAddress,
                        socketAddressLength);

  if (bindResult != 0)
  {
    printf ("Unable to bind socket to server port: %d\n", port);
    return -1;
  }

  return socketFd;
}

int server_main_loop (int socketFd)
{
  // receive request
  char buffer[UDP_BUFFER_SIZE];

  struct sockaddr_in clientAddress;

  socklen_t clientAddressLength = sizeof (clientAddress);

  memset (&clientAddress, 0, clientAddressLength);

  ssize_t receivedBytes = recvfrom (socketFd,
                                    buffer, UDP_BUFFER_SIZE, 0,
                                    (struct sockaddr *) &clientAddress,
                                    &clientAddressLength);

  if (receivedBytes == -1)
  {
    return EXIT_FAILURE;
  }

  // handle request
  // todo: parse incoming message and add some server logic
  char *message = LOREM_MESSAGE;
  size_t messageLength = strlen (message);

  // send response
  ssize_t sentBytes = server_send_message (socketFd,
                                           message, messageLength,
                                           (struct sockaddr *) &clientAddress,
                                           clientAddressLength);

  if (sentBytes < 0)
  {
    printf("Error sending message\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

ssize_t server_send_message (int socketFd,
                             char *message, size_t messageSize,
                             struct sockaddr *clientAddress,
                             socklen_t clientAddressLength)
{
  char sendBuffer[UDP_BUFFER_SIZE];
  char *currentMessageHead = message;

  size_t sentSize = 0;

  while (sentSize < messageSize)
  {
    char *sendBufferHead = sendBuffer;
    size_t chunkSize = 0;
    // copy chunk to sendBuffer

    // insert size prefix
    if (sentSize == 0)
    {
      int sizeHeader = htonl ((uint32_t) messageSize);
      memcpy (sendBufferHead, &sizeHeader, 4);
      chunkSize += 4;
      sendBufferHead += 4;
    }

    size_t remainingSize = messageSize - sentSize;
    size_t messageChunkSize;

    if (remainingSize > UDP_BUFFER_SIZE - chunkSize)
    {
      messageChunkSize = UDP_BUFFER_SIZE - chunkSize;
    } else {
      messageChunkSize = remainingSize;
    }

    memcpy (sendBufferHead, currentMessageHead, messageChunkSize);

    currentMessageHead += messageChunkSize;
    chunkSize += messageChunkSize;

    // send chunk
    ssize_t sentBytes = sendto (socketFd,
                                sendBuffer, chunkSize, 0,
                                clientAddress,
                                clientAddressLength);

    // handle errors
    if (sentBytes != chunkSize)
    {
      return -1;
    }

    // increase sentSize
    sentSize += sentBytes;
  }

  return sentSize;
}
