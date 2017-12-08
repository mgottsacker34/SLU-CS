#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// #define PORT 8889

// #define MY_SOCK_PATH "COOL_PATH"
#define bufferSize 2048
#define thread_buffer 2048

struct thread_struct{
  int fd;
  char *buffer;
};

void* read_thread(void* args) {
  struct thread_struct* arg_ptr = (struct thread_struct*) args;
  for(;;) {
    memset(arg_ptr->buffer, 0, bufferSize);
    int read_client = read(arg_ptr->fd, arg_ptr->buffer, bufferSize-1);
    if(read_client == -1) {
      perror("error calling read");
      printf("exiting program\n");
      exit(-1);
    }
    printf("%s", arg_ptr->buffer);
  }
  return NULL;
}

void* write_thread(void* args) {
  struct thread_struct* arg_ptr = (struct thread_struct*) args;
  for (;;) {
    int reader = read(STDIN_FILENO, arg_ptr->buffer, bufferSize);
    if (reader==0) {
      break;
    }
    int quitCmp = strncmp(arg_ptr->buffer, "quit\n", 5);
    // int exitCmp = strncmp(arg_ptr->buffer, "exit\n", 5);

    write(arg_ptr->fd, arg_ptr->buffer, reader);

    if(quitCmp == 0) {
      // printf("`quit` signal received.  Disconnecting user and shutting down server.\n");
      printf("`quit` signal received. Disconnecting user.\n");
      exit(0);
      // break;
    }
    // if(exitCmp == 0) {
    //   printf("`exit` signal received. Disconnecting user.\n");
    //   break;
    // }
  }
  return NULL;
}

int main( int argc, char* argv[] ) {

  if(argc != 3) {
    printf("Program requires 2 arguments. Proper syntax: ./client <ip address> <port>\nExiting.\n");
    exit(-1);
  }

  char* ADDR = (argv[1]);
  int PORT = atoi(argv[2]);

  char buffer[bufferSize];
  char buffer2[bufferSize];

  //open socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("error calling socket");
  }

  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_in));  //Clears structure

  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr(ADDR);
  client_addr.sin_port = htons(PORT);
  // strncpy(client_addr.sin_path, MY_SOCK_PATH, sizeof(client_addr.sin_path) - 1);

  //call connect
  int client_connect = connect(client_socket, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_in));

  struct thread_struct read_struct;
  struct thread_struct *read_struct_ptr = &read_struct;
  struct thread_struct write_struct;
  struct thread_struct *write_struct_ptr = &write_struct;
  pthread_t reading_thread, writing_thread;


  read_struct_ptr->fd = client_socket;
  read_struct_ptr->buffer = buffer2;
  int create_reader = pthread_create(&reading_thread, NULL, read_thread, &read_struct);
  if (create_reader != 0) {
    printf("Error creating read thread. Error #%d. Exiting.", create_reader);
    exit(-1);
  }

  write_struct_ptr->fd = client_socket;
  write_struct_ptr->buffer = buffer;
  int create_writer = pthread_create(&writing_thread, NULL, write_thread, &write_struct);
  if (create_writer != 0) {
    printf("Error creating write thread. Error #%d. Exiting.", create_writer);
    exit(-1);
  }

  int read_join_ret = pthread_join(reading_thread, NULL);
  if(read_join_ret != 0) {
    printf("Error join read thread. Error #%d. Exiting.\n", read_join_ret);
    exit(-1);
  }

  int write_join_ret = pthread_join(writing_thread, NULL);
  if(write_join_ret != 0) {
    printf("Error join write thread. Error #%d. Exiting.\n", write_join_ret);
    exit(-1);
  }

  return 0;

}
