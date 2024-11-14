#include "protocol.h"

Message *create_message(const char *type, cJSON *data) {
  Message *msg = (Message *)malloc(sizeof(Message));
  msg->type = strdup(type);
  msg->data = data;
  return msg;
}

Message *parse_message(const char *json_str) {
  printf("Phân tích thông điệp JSON: %s\n", json_str);
  cJSON *json = cJSON_Parse(json_str);
  if (!json) {
    printf("Lỗi khi phân tích JSON.\n");
    return NULL;
  }

  cJSON *type_item = cJSON_GetObjectItem(json, "type");
  cJSON *data_item = cJSON_GetObjectItem(json, "data");
  if (!type_item || !cJSON_IsString(type_item) || !data_item) {
    cJSON_Delete(json);
    printf("Thông điệp JSON không hợp lệ.\n");
    return NULL;
  }

  Message *msg = (Message *)malloc(sizeof(Message));
  msg->type = strdup(type_item->valuestring);

  // Tách data_item ra khỏi json
  cJSON_DetachItemViaPointer(json, data_item);
  msg->data = data_item;

  cJSON_Delete(json); // Xóa json gốc

  printf("Loại thông điệp: %s\n", msg->type);
  return msg;
}

void free_message(Message *msg) {
  if (msg) {
    free(msg->type);
    if (msg->data) {
      cJSON_Delete(msg->data);
    }
    free(msg);
  }
}

char *message_to_json(const Message *msg) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "type", msg->type);

  // Duplicate msg->data to avoid modifying the original
  cJSON_AddItemToObject(json, "data", cJSON_Duplicate(msg->data, 1));

  char *json_str = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);

  return json_str;
}
