/**
 * @file main.c
 * @brief Chương trình khởi động client và kết nối tới server.
 *
 * File này chứa hàm `main()`, điểm bắt đầu của chương trình client.
 * Hàm này sẽ xử lý tham số dòng lệnh để lấy địa chỉ IP và cổng server,
 * sau đó gọi hàm `start_client()` để kết nối và tương tác với server.
 */

#include "client.h"
#include <stdlib.h>

/**
 * @brief Điểm bắt đầu của chương trình client.
 *
 * Hàm `main()` xử lý các tham số dòng lệnh nhận được:
 * - Tham số thứ nhất (argv[1]) nếu có, dùng làm địa chỉ IP của server.
 * - Tham số thứ hai (argv[2]) nếu có, dùng làm cổng kết nối đến server.
 *
 * Mặc định:
 * - Địa chỉ IP: 127.0.0.1
 * - Cổng: 12345
 *
 * Sau khi xác định địa chỉ IP và cổng, hàm gọi `start_client()` để thiết lập kết nối và hiển thị
 * giao diện lệnh tương tác với server.
 *
 * @param[in] argc Số lượng tham số dòng lệnh.
 * @param[in] argv Mảng các chuỗi tương ứng với từng tham số.
 *                  - argv[1] (tùy chọn): Địa chỉ IP server.
 *                  - argv[2] (tùy chọn): Cổng server.
 *
 * @return Trả về mã thoát chương trình (0 nếu thành công, có thể khác 0 nếu xảy ra lỗi).
 *
 * @note Ví dụ chạy:
 * @code
 * ./client           // Kết nối đến 127.0.0.1:12345
 * ./client 192.168.1.10 8080 // Kết nối đến 192.168.1.10:8080
 * @endcode
 */
int main(int argc, char *argv[]) {
  char *server_ip = "127.0.0.1"; // Địa chỉ IP mặc định
  int server_port = 12345;       // Cổng mặc định

  if (argc > 1) {
    server_ip = argv[1];
  }
  if (argc > 2) {
    server_port = atoi(argv[2]);
  }

  start_client(server_ip, server_port);

  return 0;
}
