

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "client.h"
#include "user.h"
#include "encryption.h"
#include "logger.h"

#define MAX_IP_LENGTH 46
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_MESSAGE_LENGTH 1024

static int client_socket;
static char username[MAX_USERNAME_LENGTH];
static volatile int running = 1;

static void *receive_messages(void *arg)
{
    char buffer[MAX_MESSAGE_LENGTH];
    int read_size;

    while (running)
    {
        memset(buffer, 0, sizeof(buffer));
        read_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (read_size > 0)
        {
            buffer[read_size] = '\0';
            char decrypted_msg[MAX_MESSAGE_LENGTH];
            custom_decrypt(buffer, decrypted_msg);
            printf("%s\n", decrypted_msg);
        }
        else if (read_size == 0)
        {
            log_warning("Server disconnected");
            break;
        }
        else
        {
            log_error("recv failed");
            break;
        }
    }

    log_info("Receive thread ending");
    return NULL;
}

void run_client()
{
    char SERVER_IP[MAX_IP_LENGTH];
    int PORT;
    struct sockaddr_in server_addr;

    log_info("Starting client");

    printf("Enter server IP: ");
    if (fgets(SERVER_IP, sizeof(SERVER_IP), stdin) == NULL)
    {
        log_error("Error reading server IP");
        return;
    }
    SERVER_IP[strcspn(SERVER_IP, "\n")] = 0;

    printf("Enter server port: ");
    char port_str[6];
    if (fgets(port_str, sizeof(port_str), stdin) == NULL)
    {
        log_error("Error reading port");
        return;
    }
    PORT = atoi(port_str);
    if (PORT <= 0 || PORT > 65535)
    {
        log_error("Invalid port number");
        return;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        log_fatal("Error creating client socket");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        log_fatal("Error connecting to server");
        close(client_socket);
        return;
    }

    log_info("Connected to server");

    printf("Enter username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        log_error("Error reading username");
        close(client_socket);
        return;
    }
    username[strcspn(username, "\n")] = 0;

    char password[MAX_PASSWORD_LENGTH];
    printf("Enter password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        log_error("Error reading password");
        close(client_socket);
        return;
    }
    password[strcspn(password, "\n")] = 0;

    printf("Do you want to register as a new user? (y/n): ");
    char register_choice[2];
    if (fgets(register_choice, sizeof(register_choice), stdin) == NULL)
    {
        log_error("Error reading registration choice");
        close(client_socket);
        return;
    }
    if (register_choice[0] == 'y' || register_choice[0] == 'Y')
    {
        if (register_user(username, password))
        {
            log_info("User registered successfully");
        }
        else
        {
            log_error("User registration failed. User may already exist.");
            close(client_socket);
            return;
        }
    }

    if (!authenticate_user(username, password))
    {
        log_warning("Authentication failed");
        close(client_socket);
        return;
    }

    log_info("Authentication successful");

    char username_msg[MAX_MESSAGE_LENGTH];
    snprintf(username_msg, sizeof(username_msg), "USERNAME:%s", username);
    send(client_socket, username_msg, strlen(username_msg), 0);

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, NULL) != 0)
    {
        log_error("Error creating receive thread");
        close(client_socket);
        return;
    }

    char message[MAX_MESSAGE_LENGTH];
    while (running)
    {
        if (fgets(message, sizeof(message), stdin) == NULL)
        {
            log_error("Error reading message");
            break;
        }
        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "exit") == 0)
        {
            running = 0;
            break;
        }

        char encrypted_msg[MAX_MESSAGE_LENGTH];
        custom_encrypt(message, encrypted_msg);
        send(client_socket, encrypted_msg, strlen(encrypted_msg), 0);
    }

    log_info("Closing client connection");
    close(client_socket);
    pthread_cancel(receive_thread);
    pthread_join(receive_thread, NULL);
}