#include "user.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX 1024

void handle_client(int new_sock) {
  char buffer[MAX];
  while (1) {
    bzero(buffer, MAX);
    read(new_sock, buffer, sizeof(buffer));

    // Kiểm tra lệnh "register"
    if (strncmp(buffer, "register", 8) == 0) {
      char username[MAX], password[MAX];
      sscanf(buffer + 9, "%s %s", username, password);

      // Kiểm tra username và password không được rỗng
      if (strlen(username) == 0 || strlen(password) == 0) {
        char *msg = "Error: Username and password cannot be empty.\n";
        write(new_sock, msg, strlen(msg));
      } else if (check_user_exists(username)) {
        char *msg = "Error: Username already exists.\n";
        write(new_sock, msg, strlen(msg));
      } else {
        register_user(username, password);
        char *msg = "Registration successful.\n";
        write(new_sock, msg, strlen(msg));
      }
    }
    // Kiểm tra lệnh "login"
    else if (strncmp(buffer, "login", 5) == 0) {
      char username[MAX], password[MAX];
      sscanf(buffer + 6, "%s %s", username, password);

      // Kiểm tra username và password không được rỗng
      if (strlen(username) == 0 || strlen(password) == 0) {
        char *msg = "Error: Username and password cannot be empty.\n";
        write(new_sock, msg, strlen(msg));
      } else if (login_user(username, password)) {
        char *msg = "Login successful.\n";
        write(new_sock, msg, strlen(msg));
      } else {
        char *msg = "Login failed.\n";
        write(new_sock, msg, strlen(msg));
      }
    }
    // Xử lý lệnh không hợp lệ
    else {
      char *msg = "Error: Unknown command.\n";
      write(new_sock, msg, strlen(msg));
    }
  }
}

int main() {
  int sockfd, new_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Bind successful.\n");

  if (listen(sockfd, 5) != 0) {
    perror("Listen failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Server listening on port %d.\n", PORT);

  addr_size = sizeof(client_addr);
  new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
  if (new_sock < 0) {
    perror("Server accept failed");
    exit(EXIT_FAILURE);
  }
  printf("Client connected.\n");

  handle_client(new_sock);

  close(sockfd);
  return 0;
}
