// data_manager.h

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

int check_user_exists(const char *username);
int add_user(const char *username, const char *password);
int authenticate_user(const char *username, const char *password);

#endif // DATA_MANAGER_H
