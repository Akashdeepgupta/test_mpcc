

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "logger.h"
#include "server.h"

#define MAX_SERVERS 10
#define MAIN_SERVER_PORT 7000
#define MAX_COMMAND_LENGTH 256

chat_server_t *servers[MAX_SERVERS];
int server_count = 0;

void *run_chat_server(void *arg)
{
    int port = *(int *)arg;
    free(arg); // Free the allocated memory

    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "./mpcc server %d", port);
    log_info("Executing command: %s", command);
    system(command);
    return NULL;
}

// void create_chat_server(int port)
// {
//     if (server_count >= MAX_SERVERS)
//     {
//         log_error("Maximum number of servers reached");
//         return;
//     }

//     chat_server_t *server = &servers[server_count];
//     server->id = server_count + 1;
//     server->port = port;

//     int *port_ptr = malloc(sizeof(int));
//     if (port_ptr == NULL)
//     {
//         log_error("Failed to allocate memory for port");
//         return;
//     }
//     *port_ptr = port;

//     if (pthread_create(&server->thread, NULL, run_chat_server, port_ptr) != 0)
//     {
//         log_error("Failed to create chat server thread");
//         free(port_ptr);
//         return;
//     }

//     log_info("Created chat server %d on port %d", server->id, server->port);
//     server_count++;
// }

void handle_admin_commands()
{
    char command[MAX_COMMAND_LENGTH];
    while (1)
    {
        printf("Enter command (create <port> | list | exit): ");
        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            log_error("Error reading command");
            break;
        }
        command[strcspn(command, "\n")] = 0;

        log_debug("Received command: %s", command);

        if (strncmp(command, "create ", 7) == 0)
        {
            int port = atoi(command + 7);
            if (port > 0 && port <= 65535)
            {
                log_info("Attempting to create server on port %d", port);
                chat_server_t *server = create_chat_server(server_count + 1, port);
                if (server != NULL)
                {
                    servers[server_count++] = server;
                    log_info("Created chat server on port %d", port);
                }
                else
                {
                    log_error("Failed to create chat server on port %d", port);
                }
            }
            else
            {
                log_error("Invalid port number: %d", port);
                printf("Invalid port number. Please use a number between 1 and 65535.\n");
            }
        }
        else if (strcmp(command, "list") == 0)
        {
            printf("Active servers:\n");
            for (int i = 0; i < server_count; i++)
            {
                if (servers[i] != NULL)
                {
                    printf("Server ID: %d, Port: %d\n", servers[i]->id, servers[i]->port);
                }
            }
            if (server_count == 0)
            {
                printf("No active servers.\n");
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            log_info("Exiting main server");
            for (int i = 0; i < server_count; i++)
            {
                if (servers[i] != NULL)
                {
                    // You might want to implement a proper shutdown function
                    free(servers[i]);
                    servers[i] = NULL;
                }
            }
            break;
        }
        else
        {
            log_warning("Unknown command: %s", command);
            printf("Unknown command\n");
        }
    }
}

int main()
{
    if (init_logger() != 0)
    {
        fprintf(stderr, "Failed to initialize logger\n");
        return 1;
    }
    log_info("Main server started. Logging initialized.");

    log_info("Entering command loop");
    handle_admin_commands();

    log_info("Main server shutting down");
    return 0;
}