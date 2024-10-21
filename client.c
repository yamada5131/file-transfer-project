#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX 1024
#define USER_MAX 256
#define PASS_MAX 256

int main() {
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[MAX];
  char username[USER_MAX], password[PASS_MAX];

  // Socket creation
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  printf("Client socket created successfully.\n");

  // Set up server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }
  printf("Connected to server.\n");

  while (1) {
    bzero(buffer, sizeof(buffer));
    printf("Enter command (register/login): ");
    fgets(buffer, MAX, stdin);
    buffer[strlen(buffer) - 1] = '\0'; // Remove trailing newline

    if (strncmp(buffer, "register", 8) == 0) {
      printf("Enter username: ");
      fgets(username, USER_MAX, stdin);
      username[strlen(username) - 1] = '\0'; // Remove newline
      printf("Enter password: ");
      fgets(password, PASS_MAX, stdin);
      password[strlen(password) - 1] = '\0'; // Remove newline

      // Gửi lệnh "register username password" đến server
      snprintf(buffer, MAX, "register %s %s", username, password);
    } else if (strncmp(buffer, "login", 5) == 0) {
      printf("Enter username: ");
      fgets(username, USER_MAX, stdin);
      username[strlen(username) - 1] = '\0'; // Remove newline
      printf("Enter password: ");
      fgets(password, PASS_MAX, stdin);
      password[strlen(password) - 1] = '\0'; // Remove newline

      // Gửi lệnh "login username password" đến server
      snprintf(buffer, MAX, "login %s %s", username, password);
    }

    // Send the command to server
    write(sockfd, buffer, sizeof(buffer));

    bzero(buffer, sizeof(buffer));
    read(sockfd, buffer, sizeof(buffer));
    printf("Server: %s", buffer);
  }

  close(sockfd);
  return 0;
}
