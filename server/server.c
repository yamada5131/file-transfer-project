#include "server.h"
#include "session.h"
#include "handler.h"
#include "../common/network.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void *client_handler(void *arg) {
    Session *session = (Session *)arg;

    printf("Bắt đầu xử lý client.\n");

    while (1) {
        // Nhận thông điệp từ client
        Message *msg = receive_message(session->socket);
        if (!msg) {
            // Kết nối đóng hoặc lỗi
            printf("Client %s disconnected.\n", session->username);
            break;
        }

        printf("Nhận được thông điệp loại: %s\n", msg->type);

        // Xử lý yêu cầu từ client
        handle_client_request(session, msg);

        // Giải phóng thông điệp
        free_message(msg);
    }

    // Giải phóng phiên làm việc
    free_session(session);

    printf("Kết thúc xử lý client.\n");

    return NULL;
}

void start_server(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập tùy chọn SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Gán địa chỉ và cổng cho socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Gắn socket với cổng
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server đang lắng nghe trên cổng %d\n", port);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        printf("Kết nối mới từ %s:%d\n", inet_ntoa(address.sin_addr),
               ntohs(address.sin_port));

        // Tạo phiên làm việc mới cho client
        Session *session = create_session(new_socket);

        // Tạo luồng mới để xử lý client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, (void *)session) != 0) {
            perror("pthread_create");
            free_session(session);
            continue;
        }

        pthread_detach(thread_id); // Tự động thu hồi tài nguyên khi luồng kết thúc
    }

    close(server_fd);
}
