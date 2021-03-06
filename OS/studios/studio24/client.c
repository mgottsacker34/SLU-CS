#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 200

int main( int argc, char* argv[] ) {

  char buffer[ bufferSize ];

  //open socket
  int client_socket = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("error calling socket");
  }


  struct sockaddr_un client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_un));  //Clears structure

  client_addr.sun_family = AF_LOCAL;
  strncpy(client_addr.sun_path, MY_SOCK_PATH, sizeof(client_addr.sun_path) - 1);


  //call connect
  int client_connect = connect(client_socket, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_un));

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
