#include "client.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void start_client(const char *server_ip, int server_port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    // Chuyển đổi địa chỉ IPv4 và IPv6 từ text sang binary
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    // Tạo phiên làm việc
    ClientSession *session = create_client_session(sock);

    // Vòng lặp xử lý lệnh từ người dùng
    int choice;
    do {
        printf("\n=== Menu ===\n");
        printf("1. Đăng ký\n");
        printf("2. Đăng nhập\n");
        printf("3. Upload file\n");
        printf("4. Download file\n");
        printf("5. Thoát\n");
        printf("Lựa chọn của bạn: ");
        scanf("%d", &choice);
        getchar(); // Đọc ký tự xuống dòng

        switch (choice) {
            case 1:
                command_register(session);
                break;
            case 2:
                command_login(session);
                break;
            case 3:
                command_upload(session);
                break;
            case 4:
                command_download(session);
                break;
            case 5:
                printf("Thoát chương trình.\n");
                break;
            default:
                printf("Lựa chọn không hợp lệ.\n");
        }
    } while (choice != 5);

    free_client_session(session);
}
