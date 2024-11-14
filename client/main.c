#include "client.h"

int main(int argc, char *argv[]) {
  char *server_ip = "127.0.0.1"; // Địa chỉ IP mặc định
  int server_port = 12345;       // Cổng mặc định

  if (argc > 1) {
    server_ip = argv[1];
  }
  if (argc > 2) {
    server_port = atoi(argv[2]);
  }

  start_client(server_ip, server_port);

  return 0;
}
