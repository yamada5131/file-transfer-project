#ifndef SERVER_H
#define SERVER_H

#include "../common/common.h"
#include "../common/network.h"
#include "session.h"
#include "handler.h"

// Hàm khởi động server
void start_server(int port);

// Hàm xử lý client
void* client_handler(void *arg);

#endif // SERVER_H
