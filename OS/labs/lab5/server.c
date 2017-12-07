#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/time.h>


#define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 200
#define PORT 8888

struct Client{
  int fd;
  char* username;
  struct Client *next;
};

int main( int argc, char* argv[] ) {

  int quit = 0; //enables program to quit when quit is sent from client
  fd_set input_set;
  struct timeval timeout;
  int ready_for_reading = 0;
  int read_bytes = 0;
  timeout.tv_sec = 0;
  timeout.tv_usec = 100;


  struct Client *head = malloc(sizeof(struct Client));
  printf("Initialized head\n");
  head->fd = -1;
  head->username = NULL;
  head->next = NULL;
  printf("Set head->next\n");
  struct Client *tail = malloc(sizeof(struct Client));
  tail = head;

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket == -1) {
    perror("error calling socket");
    printf("exiting program\n");
    exit(0);
  }

  //setup for bind
  struct sockaddr_in serv_addr, client_addr;

  memset(&serv_addr, 0, sizeof(struct sockaddr_in));  //Clears structure
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);
  // strncpy(serv_addr.sin_path, MY_SOCK_PATH, sizeof(serv_addr.sin_path) - 1);

  int server_bind = bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
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

  //call accept
  for(;;) {
    printf("Entered main loop\n");
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    printf("Pre server accept\n");
    int server_accept = accept4(server_socket, (struct sockaddr *) &client_addr, &client_addr_size, SOCK_NONBLOCK);
    printf("Post server accept\n");
    if(server_accept == -1) {
      perror("error calling accept");
      printf("exiting program\n");
      exit(0);
    } else {
      printf("--- new connection established ---\n");
      if(head->next == NULL){
        printf("First client\n");
        head->fd = server_accept;
        printf("Server accept set\n");
        head->username = "User";
        printf("Username set\n");
        head->next = NULL;
        printf("Before tail = head\n");
        tail = head;
        printf("After tail = head\n");
      }else{
        tail = tail->next;
        tail->fd = server_accept;
        tail->username = "User";
        tail->next = NULL;
      }
    }



    for(;;) {

      //try and read from socket
      printf("Before setting current\n");
      struct Client *current = head;
      printf("After setting current\n");
      // while (current != NULL) {
        char buffer[bufferSize];
        // char *buffer_ptr = buffer;
        memset(buffer, 0, bufferSize);

        FD_ZERO(&input_set);
        FD_SET(0, &input_set);

        ready_for_reading = select(, &input_set, NULL, NULL, &timeout);
        if(ready_for_reading == -1) {
          //Nothing worth reading
        } else {
          int read_cli = read(current->fd, buffer, bufferSize-1);

          if(read_cli == -1) {
            perror("error calling read");
            printf("exiting program\n");
            exit(0);
          }

          int quitCmp = strncmp(buffer, "quit\n", 5);
          int exitCmp = strncmp(buffer, "exit\n", 5);

          if(quitCmp == 0) {
            printf("`quit` signal received.  Closing server.\n");
            quit = 1;
            break;
          }
          if(exitCmp == 0) {
            printf("--- client connection exited ---\n");
            break;
          }

          printf("%s", buffer);

        // current = current->next;
      }
    }


    if (quit == 1) {
      break;
    }
  }

  //unlink the socket/path/whatever
  // int server_unlink = unlink(MY_SOCK_PATH);
  // if(server_unlink == -1) {
  //   perror("error calling unlink");
  //   printf("exiting program\n");
  //   exit(0);
  // }

  return 0;

}
