/**
 * @file network.h
 * @brief Khai báo các hàm hỗ trợ gửi và nhận thông điệp, dữ liệu qua socket.
 *
 * File này cung cấp một tập hợp hàm để gửi và nhận thông điệp dạng JSON (được
 * định nghĩa bởi `Message`) cũng như gửi/nhận các khối dữ liệu nhị phân giữa
 * client-server thông qua socket.
 *
 */
#ifndef NETWORK_H
#define NETWORK_H

#include "protocol.h"
#include <stddef.h>

/**
 * @brief Gửi một thông điệp Message (JSON) đến socket.
 *
 * Hàm này chuyển một `Message` thành chuỗi JSON, kết thúc bằng ký tự xuống dòng
 * `\n`, sau đó gửi toàn bộ chuỗi qua socket.
 *
 * @param[in] socket  Socket đã được kết nối đến server hoặc client.
 * @param[in] msg     Con trỏ đến thông điệp `Message` cần gửi.
 *
 * @return Số byte đã gửi đi nếu thành công, hoặc -1 nếu xảy ra lỗi.
 */
int send_message(int socket, const Message *msg);
/**
 * @brief Nhận một thông điệp Message (JSON) từ socket.
 *
 * Hàm này đọc dữ liệu từ socket cho đến khi gặp ký tự xuống dòng `\n`,
 * sau đó chuyển chuỗi JSON nhận được thành đối tượng `Message`.
 *
 * @param[in] socket Socket đã được kết nối đến server hoặc client.
 *
 * @return Con trỏ đến `Message` nhận được (cần giải phóng bằng
 * `free_message()`), hoặc NULL nếu có lỗi hoặc kết nối đóng.
 */
Message *receive_message(int socket);

/**
 * @brief Gửi dữ liệu nhị phân tùy ý qua socket.
 *
 * Hàm này đảm bảo gửi toàn bộ `length` byte trong `buffer` qua socket,
 * lặp lại việc gọi `send()` đến khi tất cả dữ liệu đã được gửi.
 *
 * @param[in] socket  Socket đã kết nối.
 * @param[in] buffer  Bộ đệm chứa dữ liệu cần gửi.
 * @param[in] length  Số byte dữ liệu cần gửi.
 *
 * @return 0 nếu gửi thành công toàn bộ, -1 nếu xảy ra lỗi.
 */
int send_data(int socket, const char *buffer, size_t length);

/**
 * @brief Nhận dữ liệu nhị phân tùy ý từ socket.
 *
 * Hàm này gọi `recv()` nhiều lần để đọc đủ `length` byte từ socket và lưu vào
 * `buffer`.
 *
 * @param[in]  socket  Socket đã kết nối.
 * @param[out] buffer  Bộ đệm để lưu dữ liệu nhận được.
 * @param[in]  length  Số byte dữ liệu cần nhận.
 *
 * @return 0 nếu nhận đủ dữ liệu, -1 nếu xảy ra lỗi hoặc kết nối đóng trước khi
 * nhận đủ.
 */
int receive_data(int socket, char *buffer, size_t length);

#endif // NETWORK_H
