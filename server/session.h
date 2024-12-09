// session.h

#ifndef SESSION_H
#define SESSION_H

// Cấu trúc phiên làm việc cho server
typedef struct {
    int socket;
    int is_logged_in;
    char username[50];
} Session;

// Hàm cho server
Session *create_session(int socket);
void free_session(Session *session);

#endif // SESSION_H
