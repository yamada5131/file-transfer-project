/**
 * @file protocol.c
 * @brief Triển khai các hàm xử lý thông điệp và phản hồi định nghĩa trong
 * protocol.h.
 *
 * File này mô tả chi tiết quá trình xây dựng, phân tích và giải phóng thông
 * điệp giữa client và server. Nó cũng cho thấy cách gửi phản hồi thành công
 * hoặc thất bại.
 */

#include "protocol.h"
#include "network.h"
#include <stdlib.h>
#include <string.h>

/**
 * @internal
 * @brief Triển khai hàm `create_message()`:
 *
 * Thuật toán:
 * - Cấp phát `Message`.
 * - Sao chép chuỗi `type` bằng `strdup()`.
 * - Gán con trỏ `data` vào msg->data.
 * - Trả về con trỏ `Message` mới tạo.
 */
Message *create_message(const char *type, cJSON *data) {
  Message *msg = (Message *)malloc(sizeof(Message));
  msg->type = strdup(type);
  msg->data = data;
  return msg;
}

/**
 * @internal
 * @brief Triển khai hàm `parse_message()`:
 *
 * Thuật toán:
 * - Parse chuỗi JSON thành đối tượng cJSON.
 * - Lấy trường "type". Nếu không có hoặc không phải chuỗi, trả về NULL.
 * - Lấy trường "data" nếu có, tách ra khỏi JSON gốc để tránh bị xóa sau
 * `cJSON_Delete`.
 * - Tạo `Message` với type và data đã tách.
 * - Xóa JSON gốc và trả về `Message`.
 */
Message *parse_message(const char *json_str) {
  printf("Phân tích thông điệp JSON: %s\n", json_str);
  cJSON *json = cJSON_Parse(json_str);
  if (!json) {
    printf("Lỗi khi phân tích JSON.\n");
    return NULL;
  }

  cJSON *type_item = cJSON_GetObjectItem(json, "type");
  cJSON *data_item = cJSON_GetObjectItem(json, "data");
  if (!type_item || !cJSON_IsString(type_item)) {
    cJSON_Delete(json);
    printf("Thông điệp JSON không hợp lệ.\n");
    return NULL;
  }

  Message *msg = (Message *)malloc(sizeof(Message));
  msg->type = strdup(type_item->valuestring);

  // Tách data_item ra nếu có
  if (data_item) {
    cJSON_DetachItemViaPointer(json, data_item);
    msg->data = data_item;
  } else {
    msg->data = NULL;
  }

  cJSON_Delete(json); // Xóa json gốc

  printf("Loại thông điệp: %s\n", msg->type);
  return msg;
}

/**
 * @internal
 * @brief Triển khai hàm `free_message()`:
 *
 * Thuật toán:
 * - Giải phóng msg->type.
 * - Nếu msg->data != NULL, gọi cJSON_Delete để giải phóng JSON.
 * - Giải phóng struct Message.
 */
void free_message(Message *msg) {
  if (msg) {
    free(msg->type);
    if (msg->data) {
      cJSON_Delete(msg->data);
    }
    free(msg);
  }
}

/**
 * @internal
 * @brief Triển khai hàm `message_to_json()`:
 *
 * Thuật toán:
 * - Tạo đối tượng JSON mới.
 * - Gắn trường "type" bằng `msg->type`.
 * - Nếu có `msg->data`, sao chép vào JSON.
 * - Chuyển toàn bộ thành chuỗi JSON không định dạng (cJSON_PrintUnformatted).
 * - Trả về chuỗi JSON, cần được free sau khi dùng.
 */
char *message_to_json(const Message *msg) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "type", msg->type);

  // Nếu có data thì thêm vào
  if (msg->data) {
    cJSON_AddItemToObject(json, "data", cJSON_Duplicate(msg->data, 1));
  }

  char *json_str = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);

  return json_str;
}

/**
 * @internal
 * @brief Triển khai hàm `send_success_response()`:
 *
 * Thuật toán:
 * - Tạo cJSON với trường "status" = STATUS_OK, "message" = message.
 * - Tạo Message với type = "RESPONSE".
 * - Gửi thông điệp qua hàm send_message().
 * - Giải phóng Message.
 */
void send_success_response(int socket, const char *message) {
  cJSON *response = cJSON_CreateObject();
  cJSON_AddNumberToObject(response, "status", STATUS_OK);
  cJSON_AddStringToObject(response, "message", message);
  Message *msg = create_message("RESPONSE", response);
  send_message(socket, msg);
  free_message(msg);
}

/**
 * @internal
 * @brief Triển khai hàm `send_error_response()`:
 *
 * Thuật toán:
 * - Tạo cJSON với "status" = status_code, "message" = message.
 * - Tạo Message "RESPONSE".
 * - Gửi qua send_message().
 * - Giải phóng Message.
 */
void send_error_response(int socket, int status_code, const char *message) {
  cJSON *response = cJSON_CreateObject();
  cJSON_AddNumberToObject(response, "status", status_code);
  cJSON_AddStringToObject(response, "message", message);
  Message *msg = create_message("RESPONSE", response);
  send_message(socket, msg);
  free_message(msg);
}
