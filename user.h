#ifndef USER_H
#define USER_H

int check_user_exists(const char *username);
void register_user(const char *username, const char *password);
int login_user(const char *username, const char *password);

#endif
