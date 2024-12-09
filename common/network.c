/**
 * @file network.c
 * @brief Triển khai các hàm hỗ trợ gửi và nhận thông điệp, dữ liệu qua socket.
 *
 * File này bao gồm chi tiết triển khai của các hàm đã khai báo trong
 * `network.h`. Mặc dù người dùng API chỉ cần xem `network.h` để biết cách gọi,
 * file này cung cấp thêm thông tin nội bộ về thuật toán gửi/nhận dữ liệu.
 */

#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

/**
 * @internal
 * @brief Triển khai `send_message()`:
 *
 * Thuật toán:
 * 1. Chuyển đối tượng `Message` sang JSON bằng `message_to_json()`.
 * 2. Thêm ký tự xuống dòng `\n` vào cuối chuỗi JSON để đánh dấu kết thúc thông
 * điệp.
 * 3. Gọi `send()` để gửi toàn bộ chuỗi JSON ra socket.
 * 4. Giải phóng chuỗi JSON sau khi gửi xong.
 */
int send_message(int socket, const Message *msg) {
  char *json_str = message_to_json(msg);
  strcat(json_str, "\n"); // Thêm dấu xuống dòng
  printf("Gửi thông điệp JSON: %s\n", json_str);

  // Gửi toàn bộ chuỗi JSON ra socket
  int result = send(socket, json_str, strlen(json_str), 0);

  free(json_str);
  return result;
}

/**
 * @internal
 * @brief Triển khai `receive_message()`:
 *
 * Thuật toán:
 * 1. Đọc từng byte từ socket cho đến khi gặp ký tự xuống dòng `\n`.
 * 2. Lưu dữ liệu vào `buffer`, đảm bảo không tràn bộ đệm.
 * 3. Khi đọc xong một thông điệp JSON hoàn chỉnh, gọi `parse_message()` để
 * chuyển sang `Message`.
 * 4. Trả về `Message` hoặc NULL nếu có lỗi hay kết nối đóng.
 */
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

/**
 * @internal
 * @brief Triển khai `send_data()`:
 *
 * Thuật toán:
 * - Dùng vòng lặp gửi dữ liệu nhị phân cho đến khi toàn bộ `length` byte
 *   trong `buffer` đã được gửi hết.
 * - Nếu `send()` trả về số byte đã gửi, cập nhật `total_sent`. Nếu send lỗi
 * hoặc kết nối bị đóng, trả về -1.
 */
int send_data(int socket, const char *buffer, size_t length) {
  size_t total_sent = 0;
  while (total_sent < length) {
    ssize_t sent = send(socket, buffer + total_sent, length - total_sent, 0);
    if (sent <= 0) {
      return -1; // Gửi lỗi
    }
    total_sent += sent;
  }
  return 0;
}

/**
 * @internal
 * @brief Triển khai `receive_data()`:
 *
 * Thuật toán:
 * - Dùng vòng lặp gọi `recv()` để đọc dữ liệu cho đến khi đạt đủ `length` byte.
 * - Nếu recv trả về 0 hoặc < 0, nghĩa là kết nối đóng hoặc lỗi, trả về -1.
 * - Nếu nhận đủ dữ liệu, trả về 0.
 */
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
