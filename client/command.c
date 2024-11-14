#include "command.h"
#include "../common/network.h"
#include "../common/protocol.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

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

  // In ra dữ liệu JSON
  char *data_str = cJSON_PrintUnformatted(data);
  printf("Dữ liệu JSON gửi đi: %s\n", data_str);
  free(data_str);

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
  }
}

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
    // In ra phản hồi nhận được
    char *response_str = message_to_json(response);
    printf("Đã nhận được phản hồi từ server: %s\n", response_str);
    free(response_str);

    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      printf("Server phản hồi: %d %s\n", status_item->valueint,
             message_item->valuestring);

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

void command_upload(ClientSession *session) {
  if (!session->is_logged_in) {
    printf("Bạn cần đăng nhập trước khi upload file.\n");
    return;
  }

  char filename[256];

  printf("Nhập đường dẫn file cần upload: ");
  scanf("%s", filename);

  // Mở file để đọc dữ liệu
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    perror("Không thể mở file");
    return;
  }

  // Lấy tên file từ đường dẫn
  char *basename = strrchr(filename, '/');
  if (basename) {
    basename++;
  } else {
    basename = filename;
  }

  // Lấy kích thước file
  fseek(fp, 0L, SEEK_END);
  int64_t filesize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Tạo thông điệp upload
  cJSON *data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "filename", basename);
  cJSON_AddNumberToObject(data, "filesize", (double)filesize);

  Message *msg = create_message(MSG_TYPE_UPLOAD, data);

  printf("Đang gửi thông điệp UPLOAD đến server.\n");
  // Gửi thông điệp
  send_message(session->socket, msg);
  free_message(msg);

  printf("Đang chờ phản hồi từ server...\n");
  // Nhận phản hồi từ server
  Message *response = receive_message(session->socket);
  if (response) {
    cJSON *status_item = cJSON_GetObjectItem(response->data, "status");
    cJSON *message_item = cJSON_GetObjectItem(response->data, "message");
    if (status_item && cJSON_IsNumber(status_item) && message_item &&
        cJSON_IsString(message_item)) {
      printf("%d %s\n", status_item->valueint, message_item->valuestring);

      if (status_item->valueint == STATUS_OK) {
        // Server sẵn sàng nhận dữ liệu
        // Gửi dữ liệu file
        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
          if (send_data(session->socket, buffer, bytes_read) < 0) {
            perror("Lỗi khi gửi dữ liệu");
            break;
          }
        }
        printf("Đã gửi file thành công.\n");
      }
    }
    free_message(response);
  } else {
    printf("Không nhận được phản hồi từ server.\n");
  }

  fclose(fp);
}

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
