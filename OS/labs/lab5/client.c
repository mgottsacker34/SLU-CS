#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#define PORT 8888

#define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 200

int main( int argc, char* argv[] ) {

  char buffer[ bufferSize ];

  //open socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("error calling socket");
  }

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_in));  //Clears structure

  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = INADDR_ANY;
  client_addr.sin_port = htons(PORT);
  // strncpy(client_addr.sin_path, MY_SOCK_PATH, sizeof(client_addr.sin_path) - 1);

  //call connect
  int client_connect = connect(client_socket, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_in));

  for (;;) {
    int reader = read(STDIN_FILENO, buffer, bufferSize);
    if (reader==0) {
      break;
    }
    int quitCmp = strncmp(buffer, "quit\n", 5);
    int exitCmp = strncmp(buffer, "exit\n", 5);

    write(client_socket, buffer, reader);

    if(quitCmp == 0) {
      printf("`quit` signal received.  Disconnecting user and shutting down server.\n");
      break;
    }
    if(exitCmp == 0) {
      printf("`exit` signal received.  Disconnecting user.\n");
      break;
    }
  }

  return 0;

}
