// main.c

#include "server.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int server_port = 12349; // Cổng mặc định
  if (argc > 1) {
    server_port = atoi(argv[1]);
  }

  start_server(server_port);

  return 0;
}
