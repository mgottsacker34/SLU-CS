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
#include <arpa/inet.h>


#define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 200
// #define PORT 8889

struct Client{
  int fd;
  char* username;
  struct Client *next;
};

int main( int argc, char* argv[] ) {

  // int quit = 0; //enables program to quit when quit is sent from client
  int reaccept = 0;

  if(argc != 3) {
    printf("Program requires 2 arguments. Proper syntax: ./server <ip address> <port>\nExiting.\n");
    exit(-1);
  }

  char *ADDR = (argv[1]);
  int PORT = atoi(argv[2]);

  struct Client *head = malloc(sizeof(struct Client));
  head->fd = NULL;
  head->username = NULL;
  head->next = NULL;
  struct Client *tail = malloc(sizeof(struct Client));
  tail = head;
  struct Client *current = head;
  struct Client *current_sender = head;

  int new_client = 0;

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
  serv_addr.sin_addr.s_addr = inet_addr(ADDR);
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
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int server_accept = accept4(server_socket, (struct sockaddr *) &client_addr, &client_addr_size, SOCK_NONBLOCK);
    if(server_accept != -1) {
      if(head->fd == NULL){
        head->fd = server_accept;
        head->username = "User";
        head->next = NULL;
        tail = head;
        new_client = 1;
      } else {
        tail->next = malloc(sizeof(struct Client));
        tail->next->fd = server_accept;
        tail->next->username = "User";
        tail->next->next = NULL;
        tail = tail->next;
        new_client = 1;
      }
      printf("--- new user has joined. ---\n");
    }

    //try and read from socket
    current = head;
    while(current != NULL) {
      if (current->fd != NULL) {



        char buffer[bufferSize];
        memset(buffer, 0, bufferSize);

        int read_cli = read(current->fd, buffer, bufferSize-1);
        // if(read_cli == -1) {
        //   perror("error calling read");
        //   printf("exiting program\n");
        //   exit(0);
        // }


        int quitCmp = strncmp(buffer, "quit\n", 5);
        int nameCmp = strncmp(buffer, "name ", 5);
        int listCmp = strncmp(buffer, "list\n", 5);

        if(quitCmp == 0) {
          printf("%s has left the chat.\n", current->username);
          current->fd = NULL;
          current_sender = head;
          while(current_sender != NULL) {
            if (current_sender->fd != current->fd) {
              char *quitter = malloc(strlen(current_sender->username) + strlen(" has left the chat.\n"));
              strcpy(quitter, current->username);
              strcat(quitter, " has left the chat.\n");
              write(current_sender->fd, quitter, bufferSize);
            }
            current_sender = current_sender->next;
          }
          break;
        }

        if(nameCmp == 0){
          char *newName = buffer;
          newName = (newName+5);
          char* res = malloc(strlen(buffer-5));
          res = strncpy(res, newName, strlen(newName)-1);
          current_sender = head;
          while(current_sender != NULL) {
            if (current_sender->fd != current->fd) {
              char *name_change = malloc(strlen(current_sender->username) + strlen(" changed name to ") + strlen(res) + 1);
              strcpy(name_change, current->username);
              strcat(name_change, " changed name to ");
              strcat(name_change, res);
              strcat(name_change, "\n");
              write(current_sender->fd, name_change, bufferSize);
            }
            current_sender = current_sender->next;
          }
          printf("%s changed name to %s\n", current->username, res);
          current->username = res;

          break;
        }

        if(new_client == 1) {
          current_sender = head;
          while(current_sender != NULL) {
            if (current_sender->fd != current->fd) {
              char *joiner = malloc(strlen("--- new user has joined. ---\n"));
              strcpy(joiner, "--- new user has joined. ---\n");
              write(current_sender->fd, joiner, bufferSize);
            }
            current_sender = current_sender->next;
          }
          new_client = 0;
        }

        if (strlen(buffer) != 0) {
          printf("%s: %s", current->username, buffer);
          // printf("[%s]", buffer);
        }
        current_sender = head;
        while(current_sender != NULL) {
          if (current_sender->fd != current->fd) {
            if (strlen(buffer) != 0) {
              // printf("writing\n");
              // write(current_sender->fd, "Gottsacker sucks\n", 17);

              char *result = malloc(strlen(current->username) + strlen(buffer) + 3);
              strcpy(result, current->username);
              strcat(result, ": ");
              strcat(result, buffer);
              write(current_sender->fd, result, bufferSize);

            }
          }
          current_sender = current_sender->next;
        }

        // if (current->next == NULL) {
        //   break;
        // }
        current = current->next;
      } else {
        current = current->next;
      }
    }
  }

  printf("Broke free from loop\n");
  return 0;

}
