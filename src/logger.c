/*
 * Created by Hadar Nir on 20/07/2024.
 */

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
    time_t now;
    char time_str[20];

    time(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(stdout, "[INFO] %s: ", time_str);

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    fprintf(stdout, "\n");
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
    time_t now;
    char time_str[20];

    time(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(stderr, "[ERROR] %s: ", time_str);

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
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
    time_t now;
    char time_str[20];

    time(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(stdout, "[WARNING] %s: ", time_str);

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    fprintf(stdout, "\n");
}
