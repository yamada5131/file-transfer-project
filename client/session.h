#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include "../common/common.h"

typedef struct {
  int socket;
  int is_logged_in;
  char username[50];
  // Các thuộc tính khác nếu cần
} ClientSession;

// Hàm tạo phiên làm việc
ClientSession *create_client_session(int socket);

// Hàm giải phóng phiên làm việc
void free_client_session(ClientSession *session);

#endif // CLIENT_SESSION_H
