#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

#define SERVER_PORT 8080
#define UDP_BUFFER_SIZE 1400

int run_client (int serverSocketFd,
                struct sockaddr *serverAddress,
                socklen_t serverAddressLength);

int receiveFromServer (int serverSocketFd,
                       char **message,
                       ssize_t *messageLength);

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

  int socketFd = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in serverAddress;
  serverAddress.sin_port = SERVER_PORT;
  serverAddress.sin_family = AF_INET;

  int ipConversionResult = inet_aton(ipAddress, &serverAddress.sin_addr);

  if (ipConversionResult != 1)
  {
    printf("Invalid IP Address format. Expected valid IPv4 address (e.g. 192.168.0.1).\n");
    return EXIT_FAILURE;
  }

  socklen_t serverAddressLength = sizeof(serverAddress);

  int clientResult = run_client (socketFd,
                                 (struct sockaddr *) &serverAddress,
                                 serverAddressLength);

  if (clientResult == EXIT_SUCCESS)
  {
    printf ("Client finished.\n");
  } else {
    printf ("Error in Client.\n");
  }

  return clientResult;
}

int run_client (int serverSocketFd,
                struct sockaddr *serverAddress,
                socklen_t serverAddressLength)
{
  int connectResult = connect(serverSocketFd, serverAddress,
                              serverAddressLength);

  if (connectResult != 0)
  {
    printf("Unable to connect socket descriptor to server.\n");
    return EXIT_FAILURE;
  }

  char *requestMessage = "HEllO WORLD";

  size_t messageSize = strlen (requestMessage);

  ssize_t sentBytes = send(serverSocketFd,
                           requestMessage, messageSize, 0);

  if (sentBytes != messageSize)
  {
    printf("Unable to send message");
    return EXIT_FAILURE;
  }

  char *responseMessage = NULL;
  ssize_t messageLength = 0;

  int receiveResult = receiveFromServer (serverSocketFd,
                                         &responseMessage,
                                         &messageLength);

  if (receiveResult == EXIT_SUCCESS)
  {
    printf("Message: %s\n", responseMessage);
    printf("MessageLength: %ld\n", messageLength);
  }

  return receiveResult;
}

int receiveFromServer (int serverSocketFd,
                       char **message,
                       ssize_t *messageLength)
{
  char receiveBuffer[UDP_BUFFER_SIZE];

  int partialReceivedSize = 0;
  size_t totalMessageSize = 0;

  char *currentMessageHead = NULL;

  do {
    ssize_t receivedBytes = recv (serverSocketFd,
                                  receiveBuffer, UDP_BUFFER_SIZE, 0);

    if (receivedBytes < 0)
    {
      return EXIT_FAILURE;
    }

    size_t chunkSize = (size_t) receivedBytes;
    char *chunkHead = receiveBuffer;

    if (partialReceivedSize == 0)
    {
      if (receivedBytes < 4)
      {
        return EXIT_FAILURE;
      }

      // read length header
      uint32_t sizeHeader;
      memcpy (&sizeHeader, receiveBuffer, 4);
      chunkSize = (size_t) (receivedBytes - 4);
      chunkHead = chunkHead + 4;

      // convert from network byte order
      totalMessageSize = ntohl (sizeHeader);

      // allocate message
      *message = malloc (totalMessageSize);
      currentMessageHead = *message;
    }

    // read message chunk
    memcpy (currentMessageHead, chunkHead, chunkSize);
    currentMessageHead += chunkSize;

    partialReceivedSize += receivedBytes;
  } while (partialReceivedSize < totalMessageSize);

  *messageLength = totalMessageSize;

  return EXIT_SUCCESS;
}
