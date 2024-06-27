#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "logger.h"

#define LOG_FILE "server.log"
#define MAX_LOG_LENGTH 1024
#define TIME_BUF_SIZE 64

static FILE *log_file = NULL;

int init_logger()
{
    log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL)
    {
        perror("Failed to open log file");
        return -1;
    }
    return 0;
}

void log_message(const char *level, const char *format, va_list args)
{
    if (log_file == NULL)
    {
        fprintf(stderr, "Logger not initialized\n");
        return;
    }

    time_t now = time(NULL);
    char time_buf[TIME_BUF_SIZE];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char log_buf[MAX_LOG_LENGTH];
    int prefix_len = snprintf(log_buf, sizeof(log_buf), "[%s] %s: ", time_buf, level);
    
    if (prefix_len < 0 || prefix_len >= sizeof(log_buf))
    {
        fprintf(stderr, "Error formatting log prefix\n");
        return;
    }

    vsnprintf(log_buf + prefix_len, sizeof(log_buf) - prefix_len, format, args);
    log_buf[sizeof(log_buf) - 1] = '\0'; // Ensure null-termination

    fprintf(log_file, "%s\n", log_buf);
    fflush(log_file);
    
    printf("%s\n", log_buf);
}

void log_fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message("FATAL", format, args);
    va_end(args);
}

void log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message("ERROR", format, args);
    va_end(args);
}

void log_warning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message("WARNING", format, args);
    va_end(args);
}

void log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message("INFO", format, args);
    va_end(args);
}

void log_debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message("DEBUG", format, args);
    va_end(args);
}

void close_logger()
{
    if (log_file != NULL)
    {
        fclose(log_file);
        log_file = NULL;
    }
}
