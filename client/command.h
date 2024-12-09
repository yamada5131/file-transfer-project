/**
 * @file command.h
 * @brief Khai báo các hàm xử lý lệnh cho client.
 *
 * File này chứa khai báo các hàm cho phép client tương tác với server bằng cách
 * gửi các yêu cầu (đăng ký, đăng nhập, upload/download file và thư mục) và
 * nhận phản hồi tương ứng.
 *
 * Các hàm này dựa trên một phiên làm việc `ClientSession` để biết trạng thái
 * đăng nhập và socket kết nối tới server.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "../common/cJSON.h"
#include "session.h"

/**
 * @brief Gửi yêu cầu đăng ký tài khoản lên server.
 *
 * Hàm này yêu cầu người dùng nhập tên đăng nhập, mật khẩu, sau đó gửi thông
 * điệp đăng ký tới server. Cuối cùng, hàm nhận phản hồi từ server và in ra kết
 * quả.
 *
 * @param[in] session Phiên làm việc client.
 */
void command_register(ClientSession *session);

/**
 * @brief Gửi yêu cầu đăng nhập tài khoản lên server.
 *
 * Hàm yêu cầu người dùng nhập tên đăng nhập, mật khẩu, gửi lên server.
 * Nếu đăng nhập thành công, trường `is_logged_in` trong session sẽ được đặt
 * thành 1 và `username` được gán giá trị.
 *
 * @param[in] session Phiên làm việc client.
 */
void command_login(ClientSession *session);

/**
 * @brief Yêu cầu upload một file từ client lên server.
 *
 * Người dùng nhập đường dẫn file cần upload. Hàm này kiểm tra trạng thái đăng
 * nhập, nếu đã đăng nhập, sẽ gọi `command_upload_file()` để gửi file.
 *
 * @param[in] session Phiên làm việc client.
 */
void command_upload(ClientSession *session);

/**
 * @brief Yêu cầu download một file từ server về client.
 *
 * Người dùng nhập tên file trên server cần download. Hàm gửi yêu cầu và nhận
 * phản hồi, sau đó nhận dữ liệu file về và lưu vào đĩa.
 *
 * @param[in] session Phiên làm việc client.
 */
void command_download(ClientSession *session);

/**
 * @brief Yêu cầu upload toàn bộ một thư mục lên server.
 *
 * Người dùng nhập đường dẫn thư mục cần upload. Hàm sẽ gửi yêu cầu, và nếu
 * server chấp nhận, sẽ gửi toàn bộ nội dung thư mục (file và thư mục con) lên
 * server.
 *
 * @param[in] session Phiên làm việc client.
 */
void command_upload_directory(ClientSession *session);

/**
 * @brief Yêu cầu download toàn bộ một thư mục từ server.
 *
 * Người dùng nhập tên thư mục trên server cần download. Nếu server chấp nhận,
 * hàm sẽ nhận toàn bộ cấu trúc thư mục, tạo lại cây thư mục và file tương ứng
 * trên máy client.
 *
 * @param[in] session Phiên làm việc client.
 */
void command_download_directory(ClientSession *session);

/**
 * @brief Upload một file cụ thể lên server.
 *
 * Hàm này mở file trên client, lấy kích thước, gửi thông tin file đến server.
 * Nếu server đồng ý, gửi tiếp dữ liệu file.
 *
 * @param[in] session      Phiên làm việc client.
 * @param[in] filepath     Đường dẫn file trên máy client.
 * @param[in] dest_filename Tên file đích khi lưu trên server.
 */
void command_upload_file(ClientSession *session, const char *filepath,
                         const char *dest_filename);

/**
 * @brief Gửi toàn bộ nội dung một thư mục lên server.
 *
 * Hàm này duyệt đệ quy thư mục `dir_path`, gửi thông tin các thư mục con (tạo
 * trên server), sau đó upload các file bên trong. Cuối cùng gửi thông điệp kết
 * thúc.
 *
 * @param[in] session  Phiên làm việc client.
 * @param[in] dir_path Đường dẫn thư mục gốc cần upload.
 * @param[in] base_path Đường dẫn gốc để tính toán relative path.
 */
void send_directory(ClientSession *session, const char *dir_path,
                    const char *base_path);

/**
 * @brief Nhận toàn bộ nội dung một thư mục từ server.
 *
 * Hàm này nhận đệ quy thông điệp tạo thư mục và file từ server,
 * tạo lại cấu trúc tương tự trên máy client cho đến khi nhận được END_OF_DIR.
 *
 * @param[in] session Phiên làm việc client.
 */
void receive_directory(ClientSession *session);

/**
 * @brief Xử lý việc nhận và lưu một file được gửi từ server.
 *
 * Hàm đọc thông tin tên file, kích thước từ dữ liệu JSON, tạo thư mục nếu cần,
 * và nhận dữ liệu file lưu vào máy client.
 *
 * @param[in] session Phiên làm việc client.
 * @param[in] data    Cấu trúc JSON chứa thông tin file (filename, filesize).
 */
void handle_download_file(ClientSession *session, cJSON *data);

#endif // COMMAND_H
