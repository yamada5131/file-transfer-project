// session.c

#include "session.h"
#include <stdlib.h>
#include <unistd.h>

Session *create_session(int socket) {
  Session *session = (Session *)calloc(1, sizeof(Session));
  session->socket = socket;
  return session;
}

void free_session(Session *session) {
  if (session) {
    close(session->socket);
    free(session);
  }
}
