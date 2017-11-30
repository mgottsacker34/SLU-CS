#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 200

//structure from https://linux.die.net/man/2/bind
// struct sockaddr {
//     sa_family_t sa_family;
//     char sa_data[108];
// }

void handler(int signum){
  printf("\nReceived SIGINT. Unlinking socket and exiting.\n");
  int server_unlink = unlink(MY_SOCK_PATH);
  exit(0);
  if(server_unlink == -1) {
    perror("error calling unlink");
    printf("exiting program\n");
    exit(0);
  }
}


int main( int argc, char* argv[] ) {

  signal(2, handler);

  int server_socket = socket(AF_LOCAL, SOCK_STREAM, 0);
  if(server_socket == -1) {
    perror("error calling socket");
    printf("exiting program\n");
    exit(0);
  }

  //setup for bind
  struct sockaddr_un serv_addr, client_addr;

  memset(&serv_addr, 0, sizeof(struct sockaddr_un));  //Clears structure
  serv_addr.sun_family = AF_LOCAL;
  strncpy(serv_addr.sun_path, MY_SOCK_PATH, sizeof(serv_addr.sun_path) - 1);


  int server_bind = bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_un));
  if(server_bind == -1) {
    perror("error calling bind");
    printf("exiting program\n");
    exit(0);
  }

  //call listen
  int server_listen = listen(server_socket, 42);
  if(server_listen == -1) {
    perror("error calling listen");
    printf("exiting program\n");
    exit(0);
  }

  for(;;){

  //call accept
    socklen_t client_addr_size = sizeof(struct sockaddr_un);
    int server_accept = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size);
    if(server_accept == -1) {
      perror("error calling accept");
      printf("exiting program\n");
      exit(0);
    }else{
      printf("---new connection established---\n");
    }

    for(;;){
      //try and read from socket
      char buffer[bufferSize];
      memset(buffer, 0, bufferSize);
      int read_cli = read(server_accept, buffer, bufferSize-1);
      if(read_cli == -1) {
        perror("error calling read");
        printf("exiting program\n");
        exit(0);
      }

      int cmp = strncmp(buffer, "quit", 4);
      if(cmp == 0){
        printf("`quit` signal received.  Disconnecting that user.\n");
        break;
      }

      printf("%s", buffer);

      // FILE *server_open = fdopen(server_accept, "w+");
      // if (server_open == NULL) {
      //   perror("error calling fdopen");
      //   printf("exiting program\n");
      //   exit(0);
      // }
      //
      // char s[bufferSize];
      // fscanf(server_open, "%s", &s [0]);
      // printf("%s\n", s);

      // char buffer[bufferSize];
      // char c = fgetc(server_open);
      // while(c != EOF) {
      //   printf ("%c", c);
      //   c = fgetc(server_open);
      // }

      //unlink the socket/path/whatever
    }


  }

  int server_unlink = unlink(MY_SOCK_PATH);
  if(server_unlink == -1) {
    perror("error calling unlink");
    printf("exiting program\n");
    exit(0);
  }

  return 0;

}
