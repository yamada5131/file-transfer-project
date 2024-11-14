#include "network.h"
#include <sys/socket.h>
#include <sys/stat.h>

int send_message(int socket, const Message *msg) {
  char *json_str = message_to_json(msg);
  strcat(json_str, "\n");
  printf("Gửi thông điệp JSON: %s\n", json_str);
  int result = send(socket, json_str, strlen(json_str), 0);
  free(json_str);
  return result;
}

Message *receive_message(int socket) {
  char buffer[BUFFER_SIZE];
  int total_read = 0;
  int bytes_read;
  char *ptr = buffer;

  while ((bytes_read = recv(socket, ptr, 1, 0)) > 0) {
    if (*ptr == '\n') {
      *(ptr + 1) = '\0'; // Kết thúc chuỗi
      break;
    }
    ptr++;
    total_read++;
    if (total_read >= BUFFER_SIZE - 1) {
      // Bảo vệ tràn bộ đệm
      fprintf(stderr, "Thông điệp quá dài\n");
      return NULL;
    }
  }

  if (bytes_read <= 0) {
    // Kết nối đóng hoặc xảy ra lỗi
    return NULL;
  }

  printf("Thông điệp nhận được: %s\n", buffer);

  return parse_message(buffer);
}

int send_data(int socket, const char *buffer, size_t length) {
  size_t total_sent = 0;
  while (total_sent < length) {
    ssize_t sent = send(socket, buffer + total_sent, length - total_sent, 0);
    if (sent <= 0) {
      return -1;
    }
    total_sent += sent;
  }
  return 0;
}

int receive_data(int socket, char *buffer, size_t length) {
  size_t total_received = 0;
  while (total_received < length) {
    ssize_t received =
        recv(socket, buffer + total_received, length - total_received, 0);
    if (received <= 0) {
      return -1;
    }
    total_received += received;
  }
  return 0;
}
