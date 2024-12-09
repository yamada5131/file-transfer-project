/**
 * @file client.h
 * @brief Khai báo hàm khởi động client kết nối tới server.
 *
 * File này chứa khai báo hàm `start_client()`, dùng để khởi tạo kết nối tới
 * server và hiển thị giao diện lệnh cho người dùng.
 */

#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief Khởi động client và kết nối đến server.
 *
 * Hàm này tạo kết nối tới server thông qua địa chỉ IP và cổng được truyền vào,
 * sau đó cho phép người dùng tương tác thông qua menu. Menu sẽ thay đổi tùy
 * thuộc vào trạng thái đăng nhập:
 * - Khi chưa đăng nhập: hiển thị lựa chọn đăng ký, đăng nhập, và các tính năng
 * khác kèm cảnh báo "Cần đăng nhập trước".
 * - Khi đã đăng nhập: hiển thị chức năng upload/download file, thư mục, và thêm
 * tùy chọn đăng xuất.
 *
 * @param[in] server_ip    Địa chỉ IP của server (VD: "127.0.0.1")
 * @param[in] server_port  Cổng kết nối đến server (VD: 12345)
 *
 * @note Người dùng sẽ lặp lại việc chọn chức năng từ menu cho đến khi chọn
 * thoát.
 * @note Nếu quá trình kết nối hoặc tạo phiên làm việc thất bại, hàm in ra thông
 * báo lỗi.
 */
void start_client(const char *server_ip, int server_port);

#endif // CLIENT_H
