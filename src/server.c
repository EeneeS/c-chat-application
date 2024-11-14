#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 6969
#define MAX_BACKLOG 2

typedef struct {
  int socket;
  char name[64];
} client;

int create_socket();
int set_sock_opt(int, int);
int bind_socket(int, struct sockaddr_in *);
int listen_socket(int);
int accept_connection(int, struct sockaddr_in *, socklen_t *);
void handle_client(int);

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);

  server_fd = create_socket();
  if (server_fd < 0) {
    return -1;
  }

  if (set_sock_opt(server_fd, opt) < 0) {
    close(server_fd);
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind_socket(server_fd, &address) < 0) {
    close(server_fd);
    return -1;
  }

  if (listen_socket(server_fd) < 0) {
    close(server_fd);
    return -1;
  }

  while (1) {
    new_socket = accept_connection(server_fd, &address, &addrlen);
    if (new_socket < 0) {
      close(server_fd);
      return -1;
    }
    handle_client(new_socket);
    /*close(new_socket);*/
  };

  close(server_fd);

  return 0;
}

int create_socket() {
  int server_fd;
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket file descriptor");
    return -1;
  }
  return server_fd;
}

int set_sock_opt(int server_fd, int opt) {
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    return -1;
  }
  return 0;
}

int bind_socket(int server_fd, struct sockaddr_in *address) {
  if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
    perror("bind failed");
    return -1;
  }
  return 0;
}

int listen_socket(int server_fd) {
  if (listen(server_fd, MAX_BACKLOG) < 0) {
    perror("listen");
    return -1;
  }
  return 0;
}

int accept_connection(int server_fd, struct sockaddr_in *address,
                      socklen_t *addrlen) {
  int new_socket = accept(server_fd, (struct sockaddr *)address, addrlen);
  if (new_socket < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return new_socket;
}

void handle_client(int client_socket) {
  char buffer[64] = {0};
  char *wm = "welcome to the chatroom.";
  ssize_t valread;

  valread = read(client_socket, buffer, 63);
  if (valread < 0) {
    perror("read");
    return;
  }

  printf("%s connected to the chatroom.\n", buffer);
  send(client_socket, wm, strlen(wm), 0);
};

