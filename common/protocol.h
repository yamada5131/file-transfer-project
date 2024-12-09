/**
 * @file protocol.h
 * @brief Khai báo cấu trúc và hàm xử lý thông điệp trao đổi giữa client-server.
 *
 * File này định nghĩa các loại thông điệp, cấu trúc `Message` lưu trữ thông tin
 * dạng JSON và các hàm tạo, phân tích, giải phóng thông điệp. Đồng thời, cung
 * cấp hàm gửi phản hồi thành công hay lỗi từ server về client.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "cJSON.h"
#include "common.h"

/** @brief Loại thông điệp yêu cầu đăng ký tài khoản */
#define MSG_TYPE_REGISTER "REGISTER"
/** @brief Loại thông điệp yêu cầu đăng nhập */
#define MSG_TYPE_LOGIN "LOGIN"
/** @brief Loại thông điệp yêu cầu upload file */
#define MSG_TYPE_UPLOAD "UPLOAD"
/** @brief Loại thông điệp yêu cầu download file */
#define MSG_TYPE_DOWNLOAD "DOWNLOAD"
/** @brief Loại thông điệp yêu cầu upload thư mục */
#define MSG_TYPE_UPLOAD_DIR "UPLOAD_DIR"
/** @brief Loại thông điệp yêu cầu download thư mục */
#define MSG_TYPE_DOWNLOAD_DIR "DOWNLOAD_DIR"
/** @brief Loại thông điệp yêu cầu tạo thư mục (dùng khi upload thư mục) */
#define MSG_TYPE_MKDIR "MKDIR"
/** @brief Loại thông điệp chứa dữ liệu file */
#define MSG_TYPE_FILE "FILE"
/** @brief Loại thông điệp báo hiệu kết thúc một thư mục */
#define MSG_TYPE_END_OF_DIR "END_OF_DIR"
/** @brief Loại thông điệp yêu cầu upload một file cụ thể */
#define MSG_TYPE_UPLOAD_FILE "UPLOAD_FILE"

/**
 * @brief Cấu trúc đại diện cho một thông điệp trao đổi.
 *
 * Cấu trúc `Message` bao gồm:
 * - `type`: Chuỗi chỉ loại thông điệp (ví dụ: REGISTER, LOGIN,...).
 * - `data`: Con trỏ tới đối tượng JSON (`cJSON`) chứa dữ liệu kèm theo thông
 * điệp.
 */
typedef struct {
  char *type;  /**< Loại thông điệp */
  cJSON *data; /**< Dữ liệu JSON kèm theo (có thể NULL) */
} Message;

/**
 * @brief Tạo một thông điệp mới.
 *
 * Hàm này tạo ra một đối tượng `Message` từ loại thông điệp `type` và dữ liệu
 * `data`. Dữ liệu `data` là một đối tượng cJSON đã được tạo sẵn bên ngoài.
 *
 * @param[in] type Chuỗi mô tả loại thông điệp.
 * @param[in] data Đối tượng cJSON chứa dữ liệu thông điệp (có thể NULL).
 *
 * @return Con trỏ tới `Message` mới tạo. Cần dùng `free_message()` để giải
 * phóng khi không còn dùng.
 */
Message *create_message(const char *type, cJSON *data);

/**
 * @brief Phân tích chuỗi JSON thành một thông điệp `Message`.
 *
 * Chuỗi JSON truyền vào phải chứa ít nhất trường "type" và tuỳ chọn trường
 * "data". Hàm sẽ parse chuỗi, tạo đối tượng `Message` và tách dữ liệu JSON
 * tương ứng.
 *
 * @param[in] json_str Chuỗi JSON cần phân tích.
 *
 * @return Con trỏ tới `Message` tạo ra từ chuỗi JSON, hoặc NULL nếu parse thất
 * bại. Cần gọi `free_message()` để giải phóng khi không còn dùng.
 */
Message *parse_message(const char *json_str);

/**
 * @brief Giải phóng bộ nhớ của một `Message`.
 *
 * Hàm này giải phóng chuỗi `type`, đối tượng JSON `data` (nếu có) và chính
 * struct `Message`.
 *
 * @param[in] msg Con trỏ tới `Message` cần giải phóng.
 */
void free_message(Message *msg);

/**
 * @brief Chuyển một `Message` thành chuỗi JSON.
 *
 * Hàm tạo ra một chuỗi JSON tương ứng với `Message`. Chuỗi này cần được
 * `free()` sau khi dùng xong.
 *
 * @param[in] msg Con trỏ tới `Message`.
 *
 * @return Chuỗi JSON tương ứng, cần `free()` sau khi dùng.
 */
char *message_to_json(const Message *msg);

/**
 * @brief Gửi phản hồi thành công từ server tới client.
 *
 * Hàm này gửi một thông điệp RESPONSE kèm `status = STATUS_OK` và một thông báo
 * tùy ý.
 *
 * @param[in] socket  Socket kết nối tới client.
 * @param[in] message Thông báo phản hồi (chuỗi).
 */
void send_success_response(int socket, const char *message);

/**
 * @brief Gửi phản hồi lỗi từ server tới client.
 *
 * Hàm này gửi một thông điệp RESPONSE kèm mã lỗi `status_code` và thông báo lỗi
 * `message`.
 *
 * @param[in] socket      Socket kết nối tới client.
 * @param[in] status_code Mã lỗi (sử dụng các hằng như STATUS_OK, STATUS_ERR,
 * ...).
 * @param[in] message     Thông báo lỗi.
 */
void send_error_response(int socket, int status_code, const char *message);

#endif // PROTOCOL_H
