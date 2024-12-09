#include "handler.h"
#include "../common/common.h"
#include "../common/network.h"
#include "user.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

void handle_client_request(Session *session, const Message *msg) {
  if (strcmp(msg->type, MSG_TYPE_REGISTER) == 0) {
    printf("Gọi hàm handle_register.\n");
    handle_register(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_LOGIN) == 0) {
    printf("Gọi hàm handle_login.\n");
    handle_login(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_UPLOAD) == 0 ||
             strcmp(msg->type, MSG_TYPE_UPLOAD_FILE) == 0) {
    printf("Gọi hàm handle_upload.\n");
    handle_upload(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_DOWNLOAD) == 0) {
    printf("Gọi hàm handle_download.\n");
    handle_download(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_UPLOAD_DIR) == 0) {
    printf("Gọi hàm handle_upload_directory.\n");
    handle_upload_directory(session, msg->data);
  } else if (strcmp(msg->type, MSG_TYPE_DOWNLOAD_DIR) == 0) {
    printf("Gọi hàm handle_download_directory.\n");
    handle_download_directory(session, msg->data);
  } else {
    printf("Nhận được thông điệp không xác định.\n");
    send_error_response(session->socket, STATUS_BAD_REQUEST, "Unknown command");
  }
}

// Triển khai các hàm xử lý cụ thể...
//
int create_directories(const char *dir) {
  char tmp[512];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp), "%s", dir);
  len = strlen(tmp);
  if (tmp[len - 1] == '/')
    tmp[len - 1] = '\0';

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      if (mkdir(tmp, 0777) != 0) {
        if (errno != EEXIST) {
          perror("Cannot create directory");
          return -1;
        }
      }
      *p = '/';
    }
  }
  if (mkdir(tmp, 0777) != 0) {
    if (errno != EEXIST) {
      perror("Cannot create directory");
      return -1;
    }
  }
  return 0;
}

void handle_register(Session *session, cJSON *data) {
  printf("Xử lý yêu cầu đăng ký...\n");
  // Lấy thông tin từ data
  cJSON *username_item = cJSON_GetObjectItem(data, "username");
  cJSON *password_item = cJSON_GetObjectItem(data, "password");

  if (!username_item || !password_item || !cJSON_IsString(username_item) ||
      !cJSON_IsString(password_item)) {
    printf("username_item hoặc password_item không hợp lệ.\n");
    send_error_response(session->socket, STATUS_BAD_REQUEST,
                        "Invalid username or password");
    return;
  }

  const char *username = username_item->valuestring;
  const char *password = password_item->valuestring;

  if (check_user_exists(username)) {
    send_error_response(session->socket, STATUS_BAD_REQUEST,
                        "Username already exists");
    return;
  }

  if (add_user(username, password) == 0) {
    send_success_response(session->socket, "Register successful");
  } else {
    send_error_response(session->socket, STATUS_INTERNAL_SERVER_ERROR,
                        "Internal server error");
  }
}

void handle_login(Session *session, cJSON *data) {
  // Lấy thông tin username và password từ data
  cJSON *username_item = cJSON_GetObjectItem(data, "username");
  cJSON *password_item = cJSON_GetObjectItem(data, "password");

  if (!username_item || !password_item || !cJSON_IsString(username_item) ||
      !cJSON_IsString(password_item)) {
    send_error_response(session->socket, STATUS_BAD_REQUEST,
                        "Invalid username or password format");
    return;
  }

  const char *username = username_item->valuestring;
  const char *password = password_item->valuestring;

  if (authenticate_user(username, password)) {
    session->is_logged_in = 1;
    strcpy(session->username, username);
    send_success_response(session->socket, "Login successful");
  } else {
    send_error_response(session->socket, STATUS_UNAUTHORIZED,
                        "Invalid username or password");
  }
}

void handle_upload(Session *session, cJSON *data) {
  if (!session->is_logged_in) {
    send_error_response(session->socket, STATUS_UNAUTHORIZED,
                        "Please login first");
    return;
  }

  // Gọi hàm xử lý upload file
  handle_upload_file(session, data);
}

void handle_download(Session *session, cJSON *data) {
  if (!session->is_logged_in) {
    send_error_response(session->socket, STATUS_UNAUTHORIZED,
                        "Please login first");
    return;
  }

  // Lấy thông tin filename từ data
  cJSON *filename_item = cJSON_GetObjectItem(data, "filename");

  if (!filename_item || !cJSON_IsString(filename_item)) {
    send_error_response(session->socket, STATUS_BAD_REQUEST,
                        "Invalid filename");
    return;
  }

  const char *filename = filename_item->valuestring;

  // Xây dựng đường dẫn đầy đủ đến file
  char full_path[512];
  snprintf(full_path, sizeof(full_path), "data/%s/%s", session->username,
           filename);

  // Mở file để đọc
  FILE *fp = fopen(full_path, "rb");
  if (fp == NULL) {
    send_error_response(session->socket, STATUS_NOT_FOUND, "File not found");
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
    if (send_data(session->socket, buffer, bytes_read) < 0) {
      perror("Error sending file data");
      break;
    }
  }

  fclose(fp);
}

void handle_upload_file(Session *session, cJSON *data) {
  // Extract filename and filesize from data
  cJSON *filename_item = cJSON_GetObjectItem(data, "filename");
  cJSON *filesize_item = cJSON_GetObjectItem(data, "filesize");

  if (!filename_item || !filesize_item || !cJSON_IsString(filename_item) ||
      !cJSON_IsNumber(filesize_item)) {
    send_error_response(session->socket, STATUS_BAD_REQUEST,
                        "Invalid filename or filesize");
    return;
  }

  const char *filename = filename_item->valuestring;
  int64_t filesize = (int64_t)filesize_item->valuedouble;

  // Build the full path to save the file, including the user's directory
  char full_path[512];
  snprintf(full_path, sizeof(full_path), "data/%s/%s", session->username,
           filename);

  // Create parent directories recursively
  char dir_path[512];
  strncpy(dir_path, full_path, sizeof(dir_path));
  dir_path[sizeof(dir_path) - 1] = '\0';
  char *last_slash = strrchr(dir_path, '/');
  if (last_slash) {
    *last_slash = '\0'; // Remove filename to get directory path
    if (create_directories(dir_path) != 0) {
      send_error_response(session->socket, STATUS_INTERNAL_SERVER_ERROR,
                          "Failed to create directories on server");
      return;
    }
  }

  // Send response indicating readiness to receive the file
  send_success_response(session->socket, "Ready to receive file");

  // Receive the file data using receive_data()
  FILE *fp = fopen(full_path, "wb");
  if (fp == NULL) {
    perror("Cannot create file");
    send_error_response(session->socket, STATUS_INTERNAL_SERVER_ERROR,
                        "Failed to create file on server");
    return;
  }

  char buffer[BUFFER_SIZE];
  int64_t remaining = filesize;
  while (remaining > 0) {
    ssize_t to_read = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
    if (receive_data(session->socket, buffer, (size_t)to_read) < 0) {
      perror("Error receiving file data");
      break;
    }
    fwrite(buffer, 1, to_read, fp);
    remaining -= to_read;
  }

  fclose(fp);

  if (remaining == 0) {
    printf("File %s received successfully.\n", filename);
  } else {
    printf("Error receiving file %s.\n", filename);
  }
}

void handle_upload_directory(Session *session, cJSON *data) {
  if (!session->is_logged_in) {
    send_error_response(session->socket, STATUS_UNAUTHORIZED,
                        "Please login first");
    return;
  }

  send_success_response(session->socket, "Ready to receive directory");

  // Nhận thư mục
  receive_directory(session);
}

void handle_download_directory(Session *session, cJSON *data) {
  if (!session->is_logged_in) {
    send_error_response(session->socket, STATUS_UNAUTHORIZED,
                        "Please login first");
    return;
  }

  cJSON *dir_name_item = cJSON_GetObjectItem(data, "dir_name");
  if (!dir_name_item || !cJSON_IsString(dir_name_item)) {
    send_error_response(session->socket, STATUS_BAD_REQUEST,
                        "Invalid directory name");
    return;
  }

  // Kiểm tra thư mục có tồn tại không
  char full_path[512];
  snprintf(full_path, sizeof(full_path), "data/%s/%s", session->username,
           dir_name_item->valuestring);

  DIR *dir = opendir(full_path);
  if (!dir) {
    send_error_response(session->socket, STATUS_NOT_FOUND,
                        "Directory not found");
    return;
  }
  closedir(dir);

  send_success_response(session->socket, "Ready to send directory");

  // Gửi thư mục
  send_directory(session->socket, full_path, full_path);
}

// Hàm nhận thư mục từ client

void receive_directory(Session *session) {
  while (1) {
    Message *msg = receive_message(session->socket);
    if (!msg) {
      break;
    }

    if (strcmp(msg->type, MSG_TYPE_MKDIR) == 0) {
      cJSON *dir_name_item = cJSON_GetObjectItem(msg->data, "dir_name");
      if (dir_name_item && cJSON_IsString(dir_name_item)) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "data/%s/%s", session->username,
                 dir_name_item->valuestring);
        if (create_directories(full_path) != 0) {
          send_error_response(session->socket, STATUS_INTERNAL_SERVER_ERROR,
                              "Failed to create directories on server");
          free_message(msg);
          return;
        }
      }
      free_message(msg);

      // Sau khi MKDIR, gọi đệ quy receive_directory để xử lý thư mục con
      receive_directory(session);
      // Sau khi hàm trên kết thúc, nghĩa là đã gặp END_OF_DIR của thư mục con
      // Tiếp tục vòng lặp để xử lý tiếp các entry khác (nếu có)
      continue;
    } else if (strcmp(msg->type, MSG_TYPE_UPLOAD_FILE) == 0) {
      handle_upload_file(session, msg->data);
      free_message(msg);
    } else if (strcmp(msg->type, MSG_TYPE_END_OF_DIR) == 0) {
      free_message(msg);
      break; // Kết thúc thư mục hiện tại
    } else {
      send_error_response(session->socket, STATUS_BAD_REQUEST,
                          "Unknown command");
      free_message(msg);
    }
  }
}

// Hàm gửi thư mục tới client
void send_directory(int socket, const char *dir_path, const char *base_path) {
  DIR *dir = opendir(dir_path);
  if (!dir) {
    return;
  }

  struct dirent *entry;
  char path[1024];

  while ((entry = readdir(dir)) != NULL) {
    // Bỏ qua '.' và '..'
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

    struct stat st;
    stat(path, &st);

    if (S_ISDIR(st.st_mode)) {
      // Gửi thông điệp tạo thư mục
      cJSON *data = cJSON_CreateObject();
      cJSON_AddStringToObject(data, "dir_name", path + strlen(base_path) + 1);
      Message *msg = create_message(MSG_TYPE_MKDIR, data);
      send_message(socket, msg);
      free_message(msg);

      // Đệ quy gửi thư mục con
      send_directory(socket, path, base_path);
    } else if (S_ISREG(st.st_mode)) {
      // Gửi file
      char relative_path[1024];
      snprintf(relative_path, sizeof(relative_path), "%s",
               path + strlen(base_path) + 1);
      send_file(socket, path, relative_path);
    }
  }
  closedir(dir);

  // Gửi thông điệp kết thúc thư mục
  Message *end_msg = create_message(MSG_TYPE_END_OF_DIR, NULL);
  send_message(socket, end_msg);
  free_message(end_msg);
}

// Hàm gửi file tới client
void send_file(int socket, const char *full_path, const char *relative_path) {
  FILE *fp = fopen(full_path, "rb");
  if (fp == NULL) {
    return;
  }

  // Lấy kích thước file
  fseek(fp, 0L, SEEK_END);
  int64_t filesize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Tạo thông điệp gửi file
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "filename", relative_path);
  cJSON_AddNumberToObject(data, "filesize", (double)filesize);
  Message *msg = create_message(MSG_TYPE_FILE, data);
  send_message(socket, msg);
  free_message(msg);

  // Gửi dữ liệu file
  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    if (send_data(socket, buffer, bytes_read) < 0) {
      perror("Error sending file data");
      break;
    }
  }

  fclose(fp);
}
