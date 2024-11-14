#ifndef CLIENT_H
#define CLIENT_H

#include "../common/common.h"
#include "../common/network.h"
#include "session.h"
#include "command.h"

// Hàm khởi động client
void start_client(const char *server_ip, int server_port);

#endif // CLIENT_H
