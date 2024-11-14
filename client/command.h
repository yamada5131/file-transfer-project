#ifndef COMMAND_H
#define COMMAND_H

#include "session.h"

// Hàm xử lý lệnh đăng ký
void command_register(ClientSession *session);

// Hàm xử lý lệnh đăng nhập
void command_login(ClientSession *session);

// Hàm xử lý lệnh upload file
void command_upload(ClientSession *session);

// Hàm xử lý lệnh download file
void command_download(ClientSession *session);

// ... các hàm khác nếu cần ...

#endif // COMMAND_H
