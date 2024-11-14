#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"
#include "protocol.h"

// Hàm gửi thông điệp
int send_message(int socket, const Message *msg);

// Hàm nhận thông điệp
Message* receive_message(int socket);

// Hàm gửi dữ liệu nhị phân
int send_data(int socket, const char *buffer, size_t length);

// Hàm nhận dữ liệu nhị phân
int receive_data(int socket, char *buffer, size_t length);

#endif // NETWORK_H
