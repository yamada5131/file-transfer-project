#include "server.h"

int main(int argc, char *argv[]) {
  int server_port = 12345; // Cổng mặc định
  if (argc > 1) {
    server_port = atoi(argv[1]);
  }

  start_server(server_port);

  return 0;
}
