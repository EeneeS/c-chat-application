#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 6969

int create_socket();
int connect_to_server(int, struct sockaddr_in *);
int send_message(int, const char *);
int receive_message(int, char *, int);
void close_connection(int);

int main(int argc, char const *argv[]) {
  int client_fd;
  struct sockaddr_in serv_addr;
  char *hello = "Hello from client";
  char buffer[1024] = {0};

  client_fd = create_socket();
  if (client_fd < 0) {
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported\n");
    close_connection(client_fd);
    return -1;
  }

  if (connect_to_server(client_fd, &serv_addr) < 0) {
    close_connection(client_fd);
    return -1;
  }

  if (send_message(client_fd, hello) < 0) {
    close_connection(client_fd);
    return -1;
  }

  if (receive_message(client_fd, buffer, sizeof(buffer)) < 0) {
    close_connection(client_fd);
    return -1;
  }

  printf("%s\n", buffer);

  close_connection(client_fd);

  return 0;
}
int create_socket() {
  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }
  return client_fd;
};

int connect_to_server(int client_fd, struct sockaddr_in *serv_addr) {
  int status;
  if ((status = connect(client_fd, (struct sockaddr *)serv_addr,
                        sizeof(*serv_addr))) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }
  return 0;
};

int send_message(int client_fd, const char *message) {
  if (send(client_fd, message, strlen(message), 0) < 0) {
    printf("\nMessage send failed!\n");
    return -1;
  };
  return 1;
};

int receive_message(int client_fd, char *buffer, int buffer_size) {
  int valread = read(client_fd, buffer, buffer_size - 1);
  if (valread < 0) {
    printf("\nRead failed\n");
    return -1;
  }
  return valread;
}

void close_connection(int client_fd) { close(client_fd); };
