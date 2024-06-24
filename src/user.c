#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "user.h"
#include "encryption.h"
#include "logger.h"

#define USERS_FILE "data/registered_users.txt"

int user_exists(const char *username)
{
    FILE *file = fopen(USERS_FILE, "r");
    if (file == NULL)
    {
        log_error("Error opening users file for reading");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char *stored_username = strtok(line, ":");
        if (strcmp(username, stored_username) == 0)
        {
            fclose(file);
            return 1; // User found
        }
    }

    fclose(file);
    return 0; // User not found
}

int register_user(const char *username, const char *password)
{
    if (user_exists(username))
    {
        log_warning("User already exists");
        return 0; // Registration failed
    }
    FILE *file = fopen(USERS_FILE, "a");
    if (file == NULL)
    {
        log_error("Error opening users file");
        return 0;
    }

    char encrypted_password[100];
    custom_encrypt(password, encrypted_password);

    fprintf(file, "%s:%s\n", username, encrypted_password);
    fclose(file);

    log_info("User registered successfully");
    return 1;
}

int authenticate_user(const char *username, const char *password)
{
    FILE *file = fopen(USERS_FILE, "r");
    if (file == NULL)
    {
        log_error("Error opening users file");
        return 0;
    }

    char line[256];
    log_debug("Attempting to authenticate user");
    log_debug(username);
    while (fgets(line, sizeof(line), file))
    {
        log_debug(line);
        char *stored_username = strtok(line, ":");
        char *stored_password = strtok(NULL, "\n");

        if (strcmp(username, stored_username) == 0)
        {
            char decrypted_password[100];
            custom_decrypt(stored_password, decrypted_password);
            if (strcmp(password, decrypted_password) == 0)
            {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}
