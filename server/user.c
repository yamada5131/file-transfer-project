// data_manager.c

#include "user.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static pthread_mutex_t user_mutex = PTHREAD_MUTEX_INITIALIZER;

int check_user_exists(const char *username) {
  pthread_mutex_lock(&user_mutex);

  FILE *fp = fopen("users.txt", "r");
  if (fp == NULL) {
    pthread_mutex_unlock(&user_mutex);
    return 0;
  }

  char line[256];
  char file_username[50];
  int exists = 0;

  while (fgets(line, sizeof(line), fp)) {
    sscanf(line, "%s", file_username);
    if (strcmp(file_username, username) == 0) {
      exists = 1;
      break;
    }
  }

  fclose(fp);
  pthread_mutex_unlock(&user_mutex);
  return exists;
}

int add_user(const char *username, const char *password) {
  pthread_mutex_lock(&user_mutex);

  FILE *fp = fopen("users.txt", "a");
  if (fp == NULL) {
    pthread_mutex_unlock(&user_mutex);
    printf("Không thể mở tệp users.txt để ghi.\n");
    return -1;
  }

  fprintf(fp, "%s %s\n", username, password);
  printf("Đã thêm người dùng: %s\n", username);

  fclose(fp);
  pthread_mutex_unlock(&user_mutex);
  return 0;
}

int authenticate_user(const char *username, const char *password) {
  pthread_mutex_lock(&user_mutex);

  FILE *fp = fopen("users.txt", "r");
  if (fp == NULL) {
    pthread_mutex_unlock(&user_mutex);
    printf("Không thể mở tệp users.txt để đọc.\n");
    return 0;
  }

  char line[256];
  char file_username[50];
  char file_password[50];
  int authenticated = 0;

  while (fgets(line, sizeof(line), fp)) {
    sscanf(line, "%s %s", file_username, file_password);
    printf("Đang kiểm tra: %s %s\n", file_username, file_password);
    if (strcmp(file_username, username) == 0 &&
        strcmp(file_password, password) == 0) {
      authenticated = 1;
      break;
    }
  }

  fclose(fp);
  pthread_mutex_unlock(&user_mutex);
  printf("Kết quả xác thực cho %s: %d\n", username, authenticated);
  return authenticated;
}
