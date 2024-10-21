#include "user.h"
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_FILE "users.txt"
#define MAX 1024

// Kiểm tra người dùng đã tồn tại chưa
int check_user_exists(const char *username) {
  FILE *file = fopen(USER_FILE, "r");
  if (!file)
    return 0;
  char line[MAX];
  while (fgets(line, sizeof(line), file)) {
    char stored_username[MAX];
    sscanf(line, "%[^:]", stored_username);
    if (strcmp(stored_username, username) == 0) {
      fclose(file);
      return 1; // User exists
    }
  }
  fclose(file);
  return 0; // User does not exist
}

// Hàm đăng ký người dùng
void register_user(const char *username, const char *password) {
  FILE *file = fopen(USER_FILE, "a");
  if (!file) {
    perror("Could not open file");
    exit(EXIT_FAILURE);
  }
  // Hash the password
  char *hashed_password = crypt(password, "salt");
  fprintf(file, "%s:%s\n", username, hashed_password);
  fclose(file);
}

// Hàm đăng nhập
int login_user(const char *username, const char *password) {
  FILE *file = fopen(USER_FILE, "r");
  if (!file)
    return 0;
  char line[MAX], stored_username[MAX], stored_password[MAX];
  while (fgets(line, sizeof(line), file)) {
    sscanf(line, "%[^:]:%s", stored_username, stored_password);
    if (strcmp(stored_username, username) == 0) {
      // Hash the entered password and compare
      char *hashed_password = crypt(password, "salt");
      if (strcmp(stored_password, hashed_password) == 0) {
        fclose(file);
        return 1; // Login successful
      }
    }
  }
  fclose(file);
  return 0; // Login failed
}
