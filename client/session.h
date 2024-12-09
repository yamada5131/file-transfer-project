/**
 * @file session.h
 * @brief Khai báo cấu trúc và hàm liên quan đến phiên làm việc của client.
 *
 * File này định nghĩa cấu trúc `ClientSession` và các hàm tạo, hủy phiên làm
 * việc dành cho client kết nối tới server. Cấu trúc phiên làm việc giúp lưu trữ
 * thông tin trạng thái đăng nhập, tên người dùng, và socket kết nối đến server.
 */

#ifndef SESSION_H
#define SESSION_H

/**
 * @brief Cấu trúc phiên làm việc cho client.
 *
 * Cấu trúc này giữ thông tin:
 * - `socket`: Socket kết nối đến server.
 * - `is_logged_in`: Trạng thái đăng nhập (1 nếu đã đăng nhập, 0 nếu chưa).
 * - `username`: Tên người dùng đã đăng nhập.
 */
typedef struct {
  int socket;        /**< Socket kết nối tới server */
  int is_logged_in;  /**< Cờ trạng thái đăng nhập (1: đã đăng nhập, 0: chưa) */
  char username[50]; /**< Tên người dùng đã đăng nhập */
} ClientSession;

/**
 * @brief Tạo một phiên làm việc mới cho client.
 *
 * Hàm này khởi tạo một `ClientSession` từ socket đã kết nối đến server.
 * Phiên làm việc mới được cấp phát động, khởi tạo `socket` tương ứng,
 * và các trường khác ở trạng thái mặc định (chưa đăng nhập).
 *
 * @param[in] socket Socket đã được kết nối tới server.
 *
 * @return Con trỏ tới `ClientSession` mới được tạo. Trả về NULL nếu cấp phát
 * thất bại.
 *
 * @note Nhớ gọi `free_client_session()` khi không còn dùng phiên làm việc.
 */
ClientSession *create_client_session(int socket);

/**
 * @brief Giải phóng tài nguyên liên quan đến phiên làm việc client.
 *
 * Hàm này đóng socket được lưu trong `ClientSession` và giải phóng bộ nhớ.
 *
 * @param[in] session Con trỏ tới `ClientSession` cần giải phóng.
 *
 * @note Nếu `session` là NULL, hàm sẽ không thực hiện hành động nào.
 */
void free_client_session(ClientSession *session);

#endif // SESSION_H
