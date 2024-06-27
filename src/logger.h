#ifndef LOGGER_H
#define LOGGER_H
#include <stdarg.h>
int init_logger();
void log_fatal(const char *format, ...);
void log_error(const char *format, ...);
void log_warning(const char *format, ...);
void log_info(const char *format, ...);
void log_debug(const char *format, ...);
void close_logger();
#endif
