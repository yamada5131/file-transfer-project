#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "../common/common.h"

// Hàm kiểm tra tài khoản tồn tại
int check_user_exists(const char *username);

// Hàm thêm tài khoản mới
int add_user(const char *username, const char *password);

// Hàm xác thực người dùng
int authenticate_user(const char *username, const char *password);

// ... các hàm khác nếu cần ...

#endif // DATA_MANAGER_H
