/**
 * @file session.c
 * @brief Triển khai hàm tạo và giải phóng phiên làm việc client.
 *
 * File này bao gồm phần định nghĩa hai hàm được khai báo trong session.h:
 * - `create_client_session()`
 * - `free_client_session()`
 */

#include "session.h"
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Triển khai hàm tạo phiên làm việc client.
 *
 * Khởi tạo một `ClientSession` bằng cách cấp phát bộ nhớ động cho struct,
 * gán `socket` tương ứng, và đặt `is_logged_in = 0`, `username` rỗng.
 *
 * @param[in] socket Socket đã kết nối đến server.
 * @return Con trỏ tới `ClientSession` mới. NULL nếu cấp phát thất bại.
 */
ClientSession *create_client_session(int socket) {
  ClientSession *session = (ClientSession *)calloc(1, sizeof(ClientSession));
  if (session) {
    session->socket = socket;
  }
  return session;
}

/**
 * @brief Triển khai hàm giải phóng phiên làm việc client.
 *
 * Đóng socket trong `session` và giải phóng bộ nhớ.
 * Nếu `session` là NULL, hàm không làm gì.
 *
 * @param[in] session Con trỏ tới `ClientSession` cần giải phóng.
 */
void free_client_session(ClientSession *session) {
  if (session) {
    close(session->socket);
    free(session);
  }
}
