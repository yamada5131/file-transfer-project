#include "session.h"
#include <stdlib.h>
#include <unistd.h>

Session* create_session(int socket) {
    Session *session = (Session *)malloc(sizeof(Session));
    session->socket = socket;
    session->is_logged_in = 0;
    session->username[0] = '\0';
    return session;
}

void free_session(Session *session) {
    if (session) {
        close(session->socket);
        free(session);
    }
}
