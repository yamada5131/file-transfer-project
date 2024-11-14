#include "handler.h"
#include "data_manager.h"
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>

void handle_client_request(Session *session, const Message *msg) {
  if (strcmp(msg->type, MSG_TYPE_REGISTER) == 0) {
    printf("Gọi hàm handle_register.\n");
    handle_register(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_LOGIN) == 0) {
    printf("Gọi hàm handle_login.\n");
    handle_login(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_UPLOAD) == 0) {
    printf("Gọi hàm handle_upload.\n");
    handle_upload(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_DOWNLOAD) == 0) {
    printf("Gọi hàm handle_download.\n");
    handle_download(session, msg->data);
  } else {
    printf("Nhận được thông điệp không xác định.\n");
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_BAD_REQUEST);
    cJSON_AddStringToObject(response, "message", "Unknown command");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
  }
}

// Triển khai các hàm xử lý cụ thể...

void handle_register(Session *session, cJSON *data) {
  printf("Xử lý yêu cầu đăng ký...\n");
  printf("Nội dung data nhận được trong handle_register: %s\n",
         cJSON_PrintUnformatted(data));
  // Lấy thông tin từ data
  cJSON *username_item = cJSON_GetObjectItem(data, "username");
  cJSON *password_item = cJSON_GetObjectItem(data, "password");

  printf("username_item: %p, password_item: %p\n", (void *)username_item,
         (void *)password_item);

  if (!username_item || !password_item || !cJSON_IsString(username_item) ||
      !cJSON_IsString(password_item)) {
    printf("username_item hoặc password_item không hợp lệ.\n");
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_BAD_REQUEST);
    cJSON_AddStringToObject(response, "message",
                            "Invalid username or password");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  printf("2...\n");
  const char *username = username_item->valuestring;
  const char *password = password_item->valuestring;

  printf("Username: %s, Password: %s\n", username, password);

  if (check_user_exists(username)) {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_BAD_REQUEST);
    cJSON_AddStringToObject(response, "message", "Username already exists");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  // Thêm người dùng mới
  if (add_user(username, password) == 0) {
    // Gửi phản hồi thành công
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_OK);
    cJSON_AddStringToObject(response, "message", "Register successful");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
  } else {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_INTERNAL_SERVER_ERROR);
    cJSON_AddStringToObject(response, "message", "Internal server error");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
  }
}

void handle_login(Session *session, cJSON *data) {
  // Lấy thông tin username và password từ data
  cJSON *username_item = cJSON_GetObjectItem(data, "username");
  cJSON *password_item = cJSON_GetObjectItem(data, "password");

  if (!username_item || !password_item || !cJSON_IsString(username_item) ||
      !cJSON_IsString(password_item)) {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_BAD_REQUEST);
    cJSON_AddStringToObject(response, "message",
                            "Invalid username or password format");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  const char *username = username_item->valuestring;
  const char *password = password_item->valuestring;

  // Xác thực người dùng
  if (authenticate_user(username, password)) {
    // Cập nhật trạng thái phiên làm việc
    session->is_logged_in = 1;
    strcpy(session->username, username);

    // Gửi phản hồi thành công
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_OK);
    cJSON_AddStringToObject(response, "message", "Login successful");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
  } else {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_UNAUTHORIZED);
    cJSON_AddStringToObject(response, "message",
                            "Invalid username or password");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
  }
}

void handle_upload(Session *session, cJSON *data) {
  // Kiểm tra xem người dùng đã đăng nhập chưa
  if (!session->is_logged_in) {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_UNAUTHORIZED);
    cJSON_AddStringToObject(response, "message", "Please login first");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  // Lấy thông tin filename và filesize từ data
  cJSON *filename_item = cJSON_GetObjectItem(data, "filename");
  cJSON *filesize_item = cJSON_GetObjectItem(data, "filesize");

  if (!filename_item || !filesize_item || !cJSON_IsString(filename_item) ||
      !cJSON_IsNumber(filesize_item)) {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_BAD_REQUEST);
    cJSON_AddStringToObject(response, "message",
                            "Invalid filename or filesize");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  const char *filename = filename_item->valuestring;
  int64_t filesize = (int64_t)filesize_item->valuedouble;

  // Tạo thư mục người dùng nếu chưa tồn tại
  char user_dir[256];
  snprintf(user_dir, sizeof(user_dir), "../data/%s", session->username);
  mkdir("../data", 0777); // Tạo thư mục data nếu chưa tồn tại
  mkdir(user_dir, 0777);  // Tạo thư mục người dùng

  // Xây dựng đường dẫn đầy đủ để lưu file
  char full_path[512];
  snprintf(full_path, sizeof(full_path), "%s/%s", user_dir, filename);

  // Gửi phản hồi sẵn sàng nhận dữ liệu
  cJSON *response = cJSON_CreateObject();
  cJSON_AddNumberToObject(response, "status", STATUS_OK);
  cJSON_AddStringToObject(response, "message", "Ready to receive data");
  Message *resp_msg = create_message("RESPONSE", response);
  send_message(session->socket, resp_msg);
  free_message(resp_msg);

  // Nhận dữ liệu file
  FILE *fp = fopen(full_path, "wb");
  if (fp == NULL) {
    perror("Cannot create file");
    // Gửi phản hồi lỗi
    cJSON *error_response = cJSON_CreateObject();
    cJSON_AddNumberToObject(error_response, "status",
                            STATUS_INTERNAL_SERVER_ERROR);
    cJSON_AddStringToObject(error_response, "message",
                            "Failed to create file on server");
    Message *error_msg = create_message("RESPONSE", error_response);
    send_message(session->socket, error_msg);
    free_message(error_msg);
    return;
  }

  char buffer[BUFFER_SIZE];
  int64_t total_received = 0;
  ssize_t bytes_received;

  while (total_received < filesize) {
    bytes_received = recv(session->socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
      perror("Error receiving file data");
      break;
    }
    fwrite(buffer, 1, bytes_received, fp);
    total_received += bytes_received;
  }

  fclose(fp);

  if (total_received == filesize) {
    // Gửi phản hồi thành công
    cJSON *success_response = cJSON_CreateObject();
    cJSON_AddNumberToObject(success_response, "status", STATUS_OK);
    cJSON_AddStringToObject(success_response, "message",
                            "File uploaded successfully");
    Message *success_msg = create_message("RESPONSE", success_response);
    send_message(session->socket, success_msg);
    free_message(success_msg);
  } else {
    // Gửi phản hồi lỗi
    cJSON *error_response = cJSON_CreateObject();
    cJSON_AddNumberToObject(error_response, "status",
                            STATUS_INTERNAL_SERVER_ERROR);
    cJSON_AddStringToObject(error_response, "message", "File upload failed");
    Message *error_msg = create_message("RESPONSE", error_response);
    send_message(session->socket, error_msg);
    free_message(error_msg);
  }
}

void handle_download(Session *session, cJSON *data) {
  // Kiểm tra xem người dùng đã đăng nhập chưa
  if (!session->is_logged_in) {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_UNAUTHORIZED);
    cJSON_AddStringToObject(response, "message", "Please login first");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  // Lấy thông tin filename từ data
  cJSON *filename_item = cJSON_GetObjectItem(data, "filename");

  if (!filename_item || !cJSON_IsString(filename_item)) {
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_BAD_REQUEST);
    cJSON_AddStringToObject(response, "message", "Invalid filename");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  const char *filename = filename_item->valuestring;

  // Xây dựng đường dẫn đầy đủ đến file
  char full_path[512];
  snprintf(full_path, sizeof(full_path), "../data/%s/%s", session->username,
           filename);

  // Thêm log để in ra đường dẫn đầy đủ
  printf("Đường dẫn đầy đủ tới file: %s\n", full_path);

  // Mở file để đọc
  FILE *fp = fopen(full_path, "rb");
  if (fp == NULL) {
    // Log lỗi chi tiết
    perror("Error opening file");
    // Gửi phản hồi lỗi
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "status", STATUS_NOT_FOUND);
    cJSON_AddStringToObject(response, "message", "File not found");
    Message *resp_msg = create_message("RESPONSE", response);
    send_message(session->socket, resp_msg);
    free_message(resp_msg);
    return;
  }

  // Lấy kích thước file
  fseek(fp, 0L, SEEK_END);
  int64_t filesize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Gửi phản hồi sẵn sàng gửi dữ liệu và kích thước file
  cJSON *response = cJSON_CreateObject();
  cJSON_AddNumberToObject(response, "status", STATUS_OK);
  cJSON_AddStringToObject(response, "message", "Ready to send data");
  cJSON_AddNumberToObject(response, "filesize", (double)filesize);
  Message *resp_msg = create_message("RESPONSE", response);
  send_message(session->socket, resp_msg);
  free_message(resp_msg);

  // Gửi dữ liệu file
  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    if (send(session->socket, buffer, bytes_read, 0) < 0) {
      perror("Error sending file data");
      break;
    }
  }

  fclose(fp);
}
