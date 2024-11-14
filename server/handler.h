#ifndef HANDLER_H
#define HANDLER_H

#include "../common/common.h"
#include "../common/protocol.h"
#include "../common/network.h"
#include "session.h"
#include "data_manager.h"

// Hàm xử lý yêu cầu từ client
void handle_client_request(Session *session, const Message *msg);

// Các hàm xử lý từng loại yêu cầu
void handle_register(Session *session, cJSON *data);
void handle_login(Session *session, cJSON *data);
void handle_upload(Session *session, cJSON *data);
void handle_download(Session *session, cJSON *data);

#endif // HANDLER_H
