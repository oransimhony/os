#ifndef OSL_INCLUDE_OS_LOG_H
#define OSL_INCLUDE_OS_LOG_H

#include <stdarg.h>
#include <stdio.h>

#ifndef OSLDEF
#ifdef OSL_STATIC
#define OSLDEF static
#else
#define OSLDEF extern
#endif
#endif

typedef enum
{
    OSL_DEBUG,
    OSL_INFO,
    OSL_WARNING,
    OSL_ERROR,
    OSL_NUM_LOG_TYPES
} osl_log_type_t;

static const char *log_type_to_cstr[OSL_NUM_LOG_TYPES]
    = {[OSL_DEBUG] = "[DEBUG]",
       [OSL_INFO] = "[INFO]",
       [OSL_WARNING] = "[WARNING]",
       [OSL_ERROR] = "[ERROR]"};

OSLDEF void osl_log(osl_log_type_t log_type, const char *msg);
OSLDEF void osl_logf(osl_log_type_t log_type, const char *format, ...);

#endif

#ifdef OS_LOG_IMPLEMENETATION

OSLDEF void osl_log(osl_log_type_t log_type, const char *msg)
{
    printf("%s %s\n", log_type_to_cstr[log_type], msg);
}

OSLDEF void osl_logf(osl_log_type_t log_type, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("%s ", log_type_to_cstr[log_type]);
    vprintf(format, args);
    printf("\n");
}

#endif
