
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "client.h"
#include "logger.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <mode> [port]\n", argv[0]);
        fprintf(stderr, "mode: 'server' or 'client'\n");
        exit(1);
    }

    if (init_logger() != 0)
    {
        fprintf(stderr, "Failed to initialize logger\n");
        exit(1);
    }

    if (strcmp(argv[1], "server") == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s server <port>\n", argv[0]);
            exit(1);
        }

        int port = atoi(argv[2]);
        if (port <= 0 || port > 65535)
        {
            log_fatal("Invalid port number: %d", port);
            exit(1);
        }

        chat_server_t *server = create_chat_server(1, port);
        if (server == NULL)
        {
            log_fatal("Failed to create chat server");
            exit(1);
        }

        log_info("Chat server created on port %d", port);

        // Wait indefinitely (in a real application, you'd have a proper server management loop here)
        while (1)
        {
            sleep(1);
        }
    }
    else if (strcmp(argv[1], "client") == 0)
    {
        run_client();
    }
    else
    {
        log_fatal("Invalid mode. Use 'server' or 'client'.");
        exit(1);
    }

    return 0;
}