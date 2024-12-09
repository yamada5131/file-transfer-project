/**
 * @file command.c
 * @brief Triển khai các hàm xử lý lệnh cho client.
 *
 * File này định nghĩa các hàm đã khai báo trong `command.h`, bao gồm:
 * - Đăng ký, đăng nhập
 * - Upload, download file
 * - Upload, download thư mục
 *
 * Các hàm sử dụng các hàm hỗ trợ từ `common`, `network`, `protocol` và
 * tương tác với `ClientSession` để biết trạng thái kết nối, đăng nhập.
 */

#include "command.h"
#include "../common/common.h"
#include "../common/network.h"
#include "../common/protocol.h"
#include "session.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @internal
 * @brief Quá trình đăng ký tài khoản:
 *
 * Thuật toán:
 * - Yêu cầu người dùng nhập username, password.
 * - Tạo thông điệp JSON (username, password).
 * - Gửi thông điệp MSG_TYPE_REGISTER đến server.
 * - Nhận phản hồi: Nếu thành công, in ra thông báo; nếu thất bại, in lỗi.
 * - Không thay đổi `session->is_logged_in` ở hàm này.
 */
void command_register(ClientSession *session) {
  char username[50];
  char password[50];

  printf("Nhập tên đăng nhập: ");
  scanf("%s", username);
  printf("Nhập mật khẩu: ");
  scanf("%s", password);

  // Tạo thông điệp đăng ký
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "username", username);
  cJSON_AddStringToObject(data, "password", password);

  Message *msg = create_message(MSG_TYPE_REGISTER, data);

  printf("Đang gửi thông điệp REGISTER đến server.\n");
  // Gửi thông điệp
  send_message(session->socket, msg);
  free_message(msg);

  printf("Đang chờ phản hồi từ server...\n");
  // Nhận phản hồi
  Message *response = receive_message(session->socket);
  if (response) {
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      printf("%d %s\n", status_item->valueint, message_item->valuestring);
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }
}

/**
 * @internal
 * @brief Quá trình đăng nhập:
 *
 * Thuật toán:
 * - Hỏi username, password, gửi MSG_TYPE_LOGIN đến server.
 * - Nếu phản hồi STATUS_OK, cập nhật session->is_logged_in = 1 và
 * session->username.
 * - Nếu thất bại, in thông báo lỗi.
 */
void command_login(ClientSession *session) {
  char username[50];
  char password[50];

  printf("Nhập tên đăng nhập: ");
  scanf("%s", username);
  printf("Nhập mật khẩu: ");
  scanf("%s", password);

  // Tạo thông điệp đăng nhập
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "username", username);
  cJSON_AddStringToObject(data, "password", password);

  Message *msg = create_message(MSG_TYPE_LOGIN, data);

  // Gửi thông điệp
  printf("Đang gửi thông điệp LOGIN đến server.\n");
  send_message(session->socket, msg);
  free_message(msg);

  // Nhận phản hồi
  printf("Đang chờ phản hồi từ server...\n");
  Message *response = receive_message(session->socket);

  if (response) {
    // Xử lý phản hồi từ server
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      printf("%d %s\n", status_item->valueint, message_item->valuestring);

      if (status_item->valueint == STATUS_OK) {
        // Đăng nhập thành công
        session->is_logged_in = 1;
        strcpy(session->username, username);
        printf("Đăng nhập thành công.\n");
      } else {
        printf("Đăng nhập thất bại: %s\n", message_item->valuestring);
      }
    } else {
      printf("Phản hồi từ server không hợp lệ.\n");
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }
}

/**
 * @internal
 * @brief Quá trình upload file đơn giản:
 *
 * Thuật toán:
 * - Hỏi đường dẫn file, sau đó gọi command_upload_file() để xử lý thực tế.
 */
void command_upload(ClientSession *session) {
  if (!session->is_logged_in) {
    printf("Bạn cần đăng nhập trước khi upload file.\n");
    return;
  }

  char filename[256];

  printf("Nhập đường dẫn file cần upload: ");
  scanf("%s", filename);

  // Gửi file
  command_upload_file(session, filename, filename);
}

/**
 * @internal
 * @brief Upload một file cụ thể:
 *
 * Thuật toán:
 * - Mở file local, lấy kích thước.
 * - Gửi MSG_TYPE_UPLOAD_FILE kèm filename, filesize.
 * - Nếu server chấp nhận (STATUS_OK), gửi từng khối dữ liệu file lên server.
 * - Đóng file sau khi gửi xong.
 */
void command_upload_file(ClientSession *session, const char *filepath,
                         const char *dest_filename) {
  // Mở file để đọc dữ liệu
  FILE *fp = fopen(filepath, "rb");
  if (fp == NULL) {
    perror("Không thể mở file");
    return;
  }

  // Lấy kích thước file
  fseek(fp, 0L, SEEK_END);
  int64_t filesize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Tạo thông điệp upload
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "filename", dest_filename);
  cJSON_AddNumberToObject(data, "filesize", (double)filesize);
  Message *msg = create_message(MSG_TYPE_UPLOAD_FILE, data);

  // Gửi thông điệp
  send_message(session->socket, msg);
  free_message(msg);

  // Nhận phản hồi từ server
  Message *response = receive_message(session->socket);
  if (response) {
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    if (status_item && cJSON_IsNumber(status_item) &&
        status_item->valueint == STATUS_OK) {
      // Gửi dữ liệu file
      char buffer[BUFFER_SIZE];
      size_t bytes_read;
      while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (send_data(session->socket, buffer, bytes_read) < 0) {
          perror("Lỗi khi gửi dữ liệu");
          break;
        }
      }
      printf("Đã gửi file %s thành công.\n", dest_filename);
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }

  fclose(fp);
}

/**
 * @internal
 * @brief Quá trình download file:
 *
 * Thuật toán:
 * - Kiểm tra đăng nhập.
 * - Gửi MSG_TYPE_DOWNLOAD kèm filename đến server.
 * - Nếu được chấp nhận, nhận kích thước file, tạo file local và nhận dữ liệu
 * từng khối, ghi xuống local cho đến khi đủ kích thước.
 */
void command_download(ClientSession *session) {
  if (!session->is_logged_in) {
    printf("Bạn cần đăng nhập trước khi download file.\n");
    return;
  }

  char filename[256];
  printf("Nhập tên file cần download: ");
  scanf("%s", filename);

  // Tạo thông điệp download
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "filename", filename);
  Message *msg = create_message(MSG_TYPE_DOWNLOAD, data);

  // Gửi thông điệp
  printf("Đang gửi thông điệp DOWNLOAD đến server.\n");
  send_message(session->socket, msg);
  free_message(msg);

  // Nhận phản hồi
  printf("Đang chờ phản hồi từ server...\n");
  Message *response = receive_message(session->socket);

  if (response) {
    // Xử lý phản hồi từ server
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    cJSON *filesize_item = cJSON_GetObjectItem(response->data, "filesize");

    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      printf("%d %s\n", status_item->valueint, message_item->valuestring);

      if (status_item->valueint == STATUS_OK) {
        // Kiểm tra xem filesize_item có tồn tại không
        if (filesize_item && cJSON_IsNumber(filesize_item)) {
          int64_t filesize = (int64_t)filesize_item->valuedouble;
          printf("Kích thước file: %ld bytes\n", filesize);

          // Nhận dữ liệu file
          FILE *fp = fopen(filename, "wb");
          if (fp == NULL) {
            printf("Không thể tạo file để lưu trữ.\n");
            free_message(response);
            return;
          }

          char buffer[BUFFER_SIZE];
          int64_t total_received = 0;
          ssize_t bytes_received;

          while (total_received < filesize) {
            bytes_received = recv(session->socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
              printf("Lỗi khi nhận dữ liệu file.\n");
              break;
            }
            fwrite(buffer, 1, bytes_received, fp);
            total_received += bytes_received;
          }

          fclose(fp);

          if (total_received == filesize) {
            printf("Đã download file thành công.\n");
          } else {
            printf("Download file thất bại.\n");
          }
        } else {
          printf("Phản hồi từ server thiếu thông tin kích thước file.\n");
        }
      } else {
        // Xử lý khi server trả về lỗi
        printf("Server phản hồi lỗi: %d %s\n", status_item->valueint,
               message_item->valuestring);
      }
    } else {
      printf("Phản hồi từ server không hợp lệ.\n");
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }
}
/**
 * @internal
 * @brief Quá trình upload một thư mục:
 *
 * Thuật toán:
 * - Kiểm tra đăng nhập.
 * - Kiểm tra tồn tại của thư mục local.
 * - Gửi MSG_TYPE_UPLOAD_DIR tới server với dir_path.
 * - Nếu OK, gọi send_directory() để gửi tất cả các file và thư mục con.
 */
void command_upload_directory(ClientSession *session) {
  if (!session->is_logged_in) {
    printf("Bạn cần đăng nhập trước khi upload thư mục.\n");
    return;
  }

  char dir_path[256];
  printf("Nhập đường dẫn thư mục cần upload: ");
  scanf("%s", dir_path);

  // Check if directory exists
  DIR *dir = opendir(dir_path);
  if (!dir) {
    printf("Thư mục không tồn tại.\n");
    return;
  }
  closedir(dir);

  // Get absolute paths
  char abs_dir_path[512];
  if (realpath(dir_path, abs_dir_path) == NULL) {
    perror("realpath");
    return;
  }

  // Get parent directory
  char base_path[512];
  strncpy(base_path, abs_dir_path, sizeof(base_path));
  base_path[sizeof(base_path) - 1] = '\0';
  char *last_slash = strrchr(base_path, '/');
  if (last_slash != NULL) {
    *last_slash = '\0'; // Trim to parent directory
  } else {
    strcpy(base_path, ".");
  }

  // Send upload directory request
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "dir_path", dir_path);
  Message *msg = create_message(MSG_TYPE_UPLOAD_DIR, data);

  send_message(session->socket, msg);
  free_message(msg);

  // Receive server response
  Message *response = receive_message(session->socket);
  if (response) {
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      if (status_item->valueint == STATUS_OK) {
        // Begin sending directory
        send_directory(session, abs_dir_path, base_path);
        printf("Đã upload thư mục thành công.\n");
      } else {
        printf("Không thể upload thư mục: %s\n", message_item->valuestring);
      }
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }
}
/**
 * @internal
 * @brief Quá trình download một thư mục:
 *
 * Thuật toán:
 * - Kiểm tra đăng nhập.
 * - Gửi MSG_TYPE_DOWNLOAD_DIR với tên thư mục.
 * - Nếu server OK, gọi receive_directory() để nhận toàn bộ nội dung thư mục.
 */
void command_download_directory(ClientSession *session) {
  if (!session->is_logged_in) {
    printf("Bạn cần đăng nhập trước khi download thư mục.\n");
    return;
  }

  char dir_name[256];
  printf("Nhập tên thư mục cần download: ");
  scanf("%s", dir_name);

  // Tạo thông điệp yêu cầu download thư mục
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "dir_name", dir_name);
  Message *msg = create_message(MSG_TYPE_DOWNLOAD_DIR, data);

  // Gửi thông điệp
  send_message(session->socket, msg);
  free_message(msg);

  // Nhận phản hồi từ server
  Message *response = receive_message(session->socket);
  if (response) {
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      if (status_item->valueint == STATUS_OK) {
        // Bắt đầu nhận thư mục
        receive_directory(session);
        printf("Đã download thư mục thành công.\n");
      } else {
        printf("Không thể download thư mục: %s\n", message_item->valuestring);
      }
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }
}

/**
 * @internal
 * @brief Gửi toàn bộ nội dung thư mục:
 *
 * Thuật toán:
 * - Duyệt đệ quy các entry trong thư mục.
 * - Gửi MSG_TYPE_MKDIR cho mỗi thư mục.
 * - Gọi command_upload_file() cho mỗi file.
 * - Cuối cùng gửi MSG_TYPE_END_OF_DIR.
 */
void send_directory(ClientSession *session, const char *dir_path,
                    const char *base_path) {
  DIR *dir = opendir(dir_path);
  if (!dir) {
    return;
  }

  // Calculate relative path
  char relative_path[512];
  if (strncmp(dir_path, base_path, strlen(base_path)) == 0) {
    snprintf(relative_path, sizeof(relative_path), "%s",
             dir_path + strlen(base_path));
  } else {
    snprintf(relative_path, sizeof(relative_path), "%s", dir_path);
  }
  if (relative_path[0] == '/') {
    // Adjusted memmove usage
    size_t len = strlen(relative_path + 1) + 1; // +1 for null terminator
    memmove(relative_path, relative_path + 1, len);
  }

  // Send MKDIR for the current directory
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "dir_name", relative_path);
  Message *msg = create_message(MSG_TYPE_MKDIR, data);
  send_message(session->socket, msg);
  free_message(msg);

  // Rest of the code remains the same
  struct dirent *entry;
  char path[1024];

  while ((entry = readdir(dir)) != NULL) {
    // Skip '.' and '..'
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

    struct stat st;
    stat(path, &st);

    if (S_ISDIR(st.st_mode)) {
      // Recursive call
      send_directory(session, path, base_path);
    } else if (S_ISREG(st.st_mode)) {
      // Calculate relative path for file
      char file_relative_path[1024];
      if (strncmp(path, base_path, strlen(base_path)) == 0) {
        snprintf(file_relative_path, sizeof(file_relative_path), "%s",
                 path + strlen(base_path));
      } else {
        snprintf(file_relative_path, sizeof(file_relative_path), "%s", path);
      }
      if (file_relative_path[0] == '/') {
        // Adjusted memmove usage
        size_t file_len =
            strlen(file_relative_path + 1) + 1; // +1 for null terminator
        memmove(file_relative_path, file_relative_path + 1, file_len);
      }
      // Send file
      command_upload_file(session, path, file_relative_path);
    }
  }
  closedir(dir);

  // Send END_OF_DIR message
  Message *end_msg = create_message(MSG_TYPE_END_OF_DIR, NULL);
  send_message(session->socket, end_msg);
  free_message(end_msg);
}

/**
 * @internal
 * @brief Nhận toàn bộ nội dung thư mục:
 *
 * Thuật toán:
 * - Vòng lặp nhận message.
 * - Nếu MSG_TYPE_MKDIR: tạo thư mục local.
 * - Nếu MSG_TYPE_FILE: gọi handle_download_file() để nhận file.
 * - Nếu MSG_TYPE_END_OF_DIR: kết thúc.
 */
void receive_directory(ClientSession *session) {
  while (1) {
    Message *msg = receive_message(session->socket);
    if (!msg) {
      break;
    }

    if (strcmp(msg->type, MSG_TYPE_MKDIR) == 0) {
      cJSON *dir_name_item = cJSON_GetObjectItem(msg->data, "dir_name");
      if (dir_name_item && cJSON_IsString(dir_name_item)) {
        mkdir(dir_name_item->valuestring, 0777);
      }
    } else if (strcmp(msg->type, MSG_TYPE_FILE) == 0) {
      // Nhận file
      handle_download_file(session, msg->data);
    } else if (strcmp(msg->type, MSG_TYPE_END_OF_DIR) == 0) {
      free_message(msg);
      break;
    }
    free_message(msg);
  }
}

void handle_download_file(ClientSession *session, cJSON *data) {
  // Lấy thông tin filename và filesize từ data
  cJSON *filename_item = cJSON_GetObjectItem(data, "filename");
  cJSON *filesize_item = cJSON_GetObjectItem(data, "filesize");

  if (!filename_item || !filesize_item || !cJSON_IsString(filename_item) ||
      !cJSON_IsNumber(filesize_item)) {
    return;
  }

  const char *filename = filename_item->valuestring;
  int64_t filesize = (int64_t)filesize_item->valuedouble;

  // Tạo các thư mục cha nếu chưa tồn tại
  char *last_slash = strrchr(filename, '/');
  if (last_slash) {
    char dir_path[512];
    strncpy(dir_path, filename, last_slash - filename);
    dir_path[last_slash - filename] = '\0';

    // Tạo thư mục nếu chưa tồn tại
    char mkdir_cmd[1024];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p \"%s\"", dir_path);
    system(mkdir_cmd);
  }

  // Nhận dữ liệu file
  FILE *fp = fopen(filename, "wb");
  if (fp == NULL) {
    printf("Không thể tạo file để lưu trữ.\n");
    return;
  }

  char buffer[BUFFER_SIZE];
  int64_t total_received = 0;
  ssize_t bytes_received;

  while (total_received < filesize) {
    bytes_received = recv(session->socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
      printf("Lỗi khi nhận dữ liệu file.\n");
      break;
    }
    fwrite(buffer, 1, bytes_received, fp);
    total_received += bytes_received;
  }

  fclose(fp);
}
