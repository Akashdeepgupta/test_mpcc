#include <stdio.h>
#include <time.h>
#include "logger.h"
#include <string.h>
#define LOG_FILE "mpcc.log"
#define MAX_LOG_LENGTH 1024
static FILE *log_file = NULL;

int init_logger()
{
    log_file = fopen("server.log", "a");
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
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(log_file, "[%s] %s: ", time_buf, level);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    fflush(log_file);

    // Also print to stdout for immediate feedback
    printf("[%s] %s: ", time_buf, level);
    vprintf(format, args);
    printf("\n");

    // old stuff

    // time_t now;
    // time(&now);
    // char *date = ctime(&now);
    // date[24] = '\0'; // Remove newline

    // char message[MAX_LOG_LENGTH];
    // vsnprintf(message, sizeof(message), format, args);

    // fprintf(log_file, "[%s] %s: %s\n", date, level, message);
    // fflush(log_file);

    // printf("[%s] %s: %s\n", date, level, message);
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
