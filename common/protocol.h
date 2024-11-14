#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "cJSON.h"
#include "common.h"

// Các định nghĩa loại thông điệp
#define MSG_TYPE_REGISTER "REGISTER"
#define MSG_TYPE_LOGIN "LOGIN"
#define MSG_TYPE_UPLOAD "UPLOAD"
#define MSG_TYPE_DOWNLOAD "DOWNLOAD"

// Cấu trúc thông điệp
typedef struct {
  char *type;
  cJSON *data;
} Message;

// Hàm tạo thông điệp
Message *create_message(const char *type, cJSON *data);

// Hàm phân tích thông điệp
Message *parse_message(const char *json_str);

// Hàm giải phóng thông điệp
void free_message(Message *msg);

// Hàm chuyển thông điệp thành chuỗi JSON
char *message_to_json(const Message *msg);

#endif // PROTOCOL_H
