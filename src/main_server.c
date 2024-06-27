#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "logger.h"
#include "server.h"

#define MAX_SERVERS 100
#define MAIN_SERVER_PORT 7000
#define MAX_COMMAND_LENGTH 1024

chat_server_t *servers[MAX_SERVERS];
int server_count = 0;
pthread_mutex_t servers_mutex = PTHREAD_MUTEX_INITIALIZER;

void *run_chat_server(void *arg)
{
    int port = *(int *)arg;
    free(arg); // Free the allocated memory
	printf("The port in Run Server : %d\n",port);
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

	 // Remove newline character if present
        size_t len = strlen(command);
        if (len > 0 && command[len-1] == '\n') {
            command[len-1] = '\0';
            len--;
        }

	  log_debug("Raw command received (length %zu): '%s'", len, command);

        //command[strcspn(command, "\n")] = '\0';
	printf("command in handle admin is : %s\n",command);
        log_debug("Received command: %s", command);

//	for (size_t i = 0; i < len; i++) {
  //          log_debug("Character at position %zu: %c (ASCII: %d)", i, command[i], (int)command[i]);
    //    }

        if (strncmp(command, "create ", 7) == 0)
        {

		char *port_str = command + 7;
            log_debug("Port string: '%s'", port_str);

            // Check if port_str is empty or contains non-digit characters
            if (*port_str == '\0' || strspn(port_str, "0123456789") != strlen(port_str)) {
                log_error("Invalid port number format");
                printf("Invalid port number format. Please use only digits.\n");
                continue;
            }



            int port = atoi(command + 7);
	    log_debug("Parsed port number: %d", port);
	    printf("port in main server is : %d\n",port);
            if (port > 0 && port <= 65535)
            {
                log_info("Attempting to create server on port %d", port);
                chat_server_t *server = create_chat_server(server_count + 1, port);
            	if(server != NULL){

		    pthread_mutex_lock(&servers_mutex);
                    if (server_count < MAX_SERVERS)
                    {
                        servers[server_count++] = server;
                        log_info("Created chat server on port %d", server->port);
                    }
                    else
                    {
                        log_error("Maximum number of servers reached");
                        free(server);
                    }
                    pthread_mutex_unlock(&servers_mutex);
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
    pthread_mutex_destroy(&servers_mutex);
    return 0;
}
