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
#include <fcntl.h>


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
  // fd_set input_set;
  // struct timeval timeout;
  // int ready_for_reading = 0;
  // int read_bytes = 0;
  // timeout.tv_sec = 0;
  // timeout.tv_usec = 100;
  int reaccept = 0;



  struct Client *head = malloc(sizeof(struct Client));
  printf("Initialized head\n");
  head->fd = NULL;
  head->username = NULL;
  head->next = NULL;
  printf("Set head->next\n");
  struct Client *tail = malloc(sizeof(struct Client));
  tail = head;
  struct Client *current = head;

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket == -1) {
    perror("error calling socket");
    printf("exiting program\n");
    exit(0);
  }

  int server_fcntl = fcntl(server_socket, F_SETFL, O_NONBLOCK);
  if(server_fcntl == -1) {
    perror("error calling fcntl");
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
    // printf("Entered main loop\n");
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    // printf("Pre server accept\n");
    int server_accept = accept4(server_socket, (struct sockaddr *) &client_addr, &client_addr_size, SOCK_NONBLOCK);
    // printf("%d\n", server_accept);
    if(server_accept == -1) {
      // perror("error calling accept");
      // printf("exiting program\n");
      // exit(0);
    } else {
      printf("--- new user has joined. ---\n");
      if(head->fd == NULL){
        printf("First client\n");
        head->fd = server_accept;
        printf("New client's fd is %d\n", tail->fd);
        head->username = "User";
        printf("Username set\n");
        head->next = NULL;
        printf("Before tail = head\n");
        tail = head;
        printf("After tail = head\n");
      } else {
        printf("Pre set tail to next\n");
        // tail = tail->next;
        printf("Post set tail to next\n");
        tail->next = malloc(sizeof(struct Client));
        tail->next->fd = server_accept;
        printf("New client's fd is %d\n", tail->fd);
        tail->next->username = "User";
        tail->next->next = NULL;
        tail = tail->next;
      }
    }

    // struct Client *current = head;
    // // printf("After setting current\n");
    // while(current != NULL) {
    //   printf("current fd: %d", current->fd);
    // }



    // for(;;) {
      // printf("Dear god help me I'm trapped in this loop\n");
      //try and read from socket
      // printf("Before setting current\n");
      reaccept = 0;
      current = head;
      // printf("After setting current\n");
      while(current != NULL) {
        if (current->fd != NULL) {
          // printf("current fd: %d\n", current->fd);
          char buffer[bufferSize];
          memset(buffer, 0, bufferSize);

          int read_cli = read(current->fd, buffer, bufferSize-1);

          // if(read_cli == -1) {
          //   perror("error calling read");
          //   printf("exiting program\n");
          //   exit(0);
          // }

          int quitCmp = strncmp(buffer, "quit\n", 5);
          int exitCmp = strncmp(buffer, "exit\n", 5);
          int setNameCmp = strncmp(buffer, "name\n", 5);

          int listCmp = strncmp(buffer, "list\n", 5);

          if(quitCmp == 0) {
            printf("`quit` signal received.  Closing server.\n");
            quit = 1;
            break;
          }
          if(exitCmp == 0) {
            printf("--- %s has left the chat. ---\n", current->username);
            break;
          }
          if(setNameCmp == 0){
            write(current->fd, "Enter a new username: ", 22);
            break;
          }

          //Checking list
          if(listCmp == 0) {
            struct Client *thisone = head;
            while(thisone != NULL) {
              printf("user #%d: %s\n", thisone->fd, current->username);
              thisone = thisone->next;
            }
          }


          // printf("%s: %s", current->username, buffer);
          printf("%s", buffer);

          if (current->next == NULL) {
            break;
          }
          current = current->next;
        } else {
          current = current->next;
        }
      }
    //   if(reaccept == 1) {
    //     break;
    //   }
    // }

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
