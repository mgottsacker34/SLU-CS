#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 512

int main( int argc, char* argv[] ) {

  char buffer[ bufferSize ];
  int quit = 1;

  //open socket
  int client_socket = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("error calling socket");
  }

  //call bind
  struct sockaddr_un client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_un));  //Clears structure

  client_addr.sun_family = AF_LOCAL;
  strncpy(client_addr.sun_path, MY_SOCK_PATH, sizeof(client_addr.sun_path) - 1);

  //call connect
  int client_connect = connect(client_socket, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_un));

  for (;;) {
    memset(buffer, 0, bufferSize);
    int reader = read(STDIN_FILENO, buffer, bufferSize);
    if (reader==0) {
      break;
    }
    // printf("buffer: %s\n", buffer);

    int quitCmp = strncmp(buffer, "quit\n", 5);
    int exitCmp = strncmp(buffer, "exit\n", 5);

    write(client_socket, buffer, reader);

    if(quitCmp == 0) {
      printf("`quit` signal received.  Disconnecting user and shutting down server.\n");
      quit = 1;
      break;
    }
    if(exitCmp == 0) {
      printf("`exit` signal received.  Disconnecting user.\n");
      break;
    }

    char buffer2[512];
    memset(buffer2, 0, 512);
    // while(read(client_socket, buffer2, 512) != 0){
    //   printf("%s", buffer2);
    //   // quit = 1;
    //   break;
    // }
    for(;;){

      int read_cli = read(client_socket, buffer2, 512);
      if(read_cli == 0){
        // quit = 1;
        break;
      }
      if(read_cli == -1) {
        perror("error calling read");
        printf("exiting program\n");
        exit(0);
      }
      printf("%s", buffer2);
      // quit = 1;
      // break;
    }
    // printf("Here\n");

    if(quit == 1){
      break;
    }

  }

  if(quit == 1){
    exit(0);
  }

  return 0;

}
