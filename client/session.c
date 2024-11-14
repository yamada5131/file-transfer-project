#include "session.h"
#include <stdlib.h>
#include <unistd.h>

ClientSession *create_client_session(int socket) {
  ClientSession *session = (ClientSession *)malloc(sizeof(ClientSession));
  session->socket = socket;
  session->is_logged_in = 0;
  session->username[0] = '\0';
  return session;
}

void free_client_session(ClientSession *session) {
  if (session) {
    close(session->socket);
    free(session);
  }
}
