// handler.h

#ifndef HANDLER_H
#define HANDLER_H

#include "session.h"
#include "../common/protocol.h"

// Hàm xử lý yêu cầu từ client
void handle_client_request(Session *session, const Message *msg);

// Các hàm xử lý cụ thể
void handle_register(Session *session, cJSON *data);
void handle_login(Session *session, cJSON *data);
void handle_upload(Session *session, cJSON *data);
void handle_download(Session *session, cJSON *data);
void handle_upload_directory(Session *session, cJSON *data);
void handle_download_directory(Session *session, cJSON *data);
void handle_upload_file(Session *session, cJSON *data);
void receive_directory(Session *session);
void send_directory(int socket, const char *dir_path, const char *base_path);
void send_file(int socket, const char *full_path, const char *relative_path);

#endif // HANDLER_H
