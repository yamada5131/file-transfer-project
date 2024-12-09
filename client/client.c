/**
 * @file client.c
 * @brief Triển khai hàm khởi động client và xử lý menu tương tác với server.
 *
 * File này chứa định nghĩa của `start_client()` đã khai báo trong client.h.
 * Quá trình thực hiện gồm:
 * - Tạo và kết nối socket đến server.
 * - Tạo một `ClientSession` để quản lý trạng thái.
 * - In menu và nhận lệnh người dùng trong vòng lặp:
 *   + Nếu chưa đăng nhập: cho phép đăng ký, đăng nhập, và thông báo cần đăng
 * nhập trước khi thực hiện các chức năng khác.
 *   + Nếu đã đăng nhập: hiển thị các chức năng upload/download file/thư mục,
 *     kèm tùy chọn đăng xuất.
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "command.h"
#include "session.h"

/**
 * @brief In menu tương ứng với trạng thái đăng nhập của phiên làm việc.
 *
 * Nếu chưa đăng nhập, menu sẽ bao gồm:
 * 1. Đăng ký
 * 2. Đăng nhập
 * 3. Thoát
 *
 * Nếu đã đăng nhập, menu sẽ bao gồm:
 * 1. Upload file
 * 2. Download file
 * 3. Upload thư mục
 * 4. Download thư mục
 * 5. Đăng xuất
 * 6. Thoát
 *
 * @param[in] session Con trỏ đến phiên làm việc client.
 */
static void print_menu(const ClientSession *session) {
  printf("\n=== Menu ===\n");
  if (!session->is_logged_in) {
    printf("1. Đăng ký\n");
    printf("2. Đăng nhập\n");
    printf("3. Thoát\n");
  } else {
    printf("Đã đăng nhập với tài khoản %s\n\n", session->username);
    printf("1. Upload file\n");
    printf("2. Download file\n");
    printf("3. Upload thư mục\n");
    printf("4. Download thư mục\n");
    printf("5. Đăng xuất\n");
    printf("6. Thoát\n");
  }
  printf("Lựa chọn của bạn: ");
}

/**
 * @brief Đọc và chuyển đổi lựa chọn từ người dùng thành số nguyên.
 *
 * Hàm này:
 * - Đọc một dòng từ stdin.
 * - Kiểm tra xem dữ liệu nhập có phải toàn ký tự số không.
 * - Chuyển đổi sang kiểu int.
 *
 * @return Số người dùng nhập, hoặc -1 nếu dữ liệu không phải số hoặc đọc bị
 * lỗi.
 */
static int get_user_choice() {
  char buffer[128];
  if (!fgets(buffer, sizeof(buffer), stdin)) {
    return -1;
  }

  buffer[strcspn(buffer, "\n")] = '\0';

  for (size_t i = 0; i < strlen(buffer); i++) {
    if (!isdigit((unsigned char)buffer[i])) {
      return -1;
    }
  }

  return atoi(buffer);
}

void start_client(const char *server_ip, int server_port) {
  int sock = 0;
  struct sockaddr_in serv_addr;

  // Tạo socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\nLỗi tạo socket\n");
    return;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(server_port);

  // Chuyển đổi địa chỉ IP dạng chuỗi sang binary
  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
    printf("\nĐịa chỉ không hợp lệ hoặc không được hỗ trợ\n");
    close(sock);
    return;
  }

  // Kết nối tới server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nKết nối thất bại\n");
    close(sock);
    return;
  }

  // Tạo phiên làm việc
  ClientSession *session = create_client_session(sock);
  if (!session) {
    printf("Không thể tạo phiên làm việc.\n");
    close(sock);
    return;
  }

  int choice;
  do {
    print_menu(session);
    choice = get_user_choice();

    if (!session->is_logged_in) {
      // Chưa đăng nhập
      switch (choice) {
      case 1:
        command_register(session);
        break;
      case 2:
        command_login(session);
        break;
      case 3:
        printf("Thoát chương trình.\n");
        break;
      default:
        printf("Lựa chọn không hợp lệ.\n");
      }
    } else {
      // Đã đăng nhập
      switch (choice) {
      case 1:
        command_upload(session);
        break;
      case 2:
        command_download(session);
        break;
      case 3:
        command_upload_directory(session);
        break;
      case 4:
        command_download_directory(session);
        break;
      case 5:
        // Đăng xuất
        session->is_logged_in = 0;
        session->username[0] = '\0';
        printf("Đã đăng xuất.\n");
        break;
      case 6:
        printf("Thoát chương trình.\n");
        break;
      default:
        printf("Lựa chọn không hợp lệ.\n");
      }
    }

  } while ((!session->is_logged_in && choice != 3) ||
           (session->is_logged_in && choice != 6));

  free_client_session(session);
}
