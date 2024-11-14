#ifndef SESSION_H
#define SESSION_H

#include "../common/common.h"

typedef struct {
    int socket;
    int is_logged_in;
    char username[50];
    // Các thuộc tính khác nếu cần
} Session;

// Hàm tạo phiên làm việc mới
Session* create_session(int socket);

// Hàm giải phóng phiên làm việc
void free_session(Session *session);

#endif // SESSION_H
