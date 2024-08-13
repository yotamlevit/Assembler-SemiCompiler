
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

/**
 * @brief Logs an informational message to stdout.
 *
 * This function logs an informational message to stdout, including a timestamp
 * and a custom message. The message is formatted similarly to printf.
 *
 * @param format The format string (as in printf).
 * @param ... Additional arguments (as in printf).
 */
void info_log(const char *format, ...) {
    va_list args;

    fprintf(stdout, "\033[1;34m[INFO]: ");

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    fprintf(stdout, "\033[0m\n");
}


/**
 * @brief Logs an error message to stderr.
 *
 * This function logs an error message to stderr, including a timestamp
 * and a custom message. The message is formatted similarly to printf.
 *
 * @param format The format string (as in printf).
 * @param ... Additional arguments (as in printf).
 */
void error_log(const char *format, ...) {
    va_list args;

    fprintf(stderr, "\033[1;31m[ERROR]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\033[0m\n");
}


/**
 * @brief Logs a warning message to stdout.
 *
 * This function logs a warning message to stdout, including a timestamp
 * and a custom message. The message is formatted similarly to printf.
 *
 * @param format The format string (as in printf).
 * @param ... Additional arguments (as in printf).
 */
void warning_log(const char *format, ...) {
    va_list args;

    fprintf(stdout, "\033[1;33m[WARNING]: ");

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    fprintf(stdout, "\033[0m\n");
}
