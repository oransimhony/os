#ifndef OSSB_INCLUDE_OS_SB_H
#define OSSB_INCLUDE_OS_SB_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef OSSBDEF
#ifdef OSSB_STATIC
#define OSSBDEF static
#else
#define OSSBDEF extern
#endif
#endif

#ifndef OSSB_INITIAL_CAPACITY
#define OSSB_INITIAL_CAPACITY (256)
#endif

#ifndef OSSB_FACTOR
#define OSSB_FACTOR (2)
#endif

typedef struct
{
    char *buffer;
    size_t capacity;
    size_t length;
} os_string_builder_t;

OSSBDEF os_string_builder_t sb_new(void);
OSSBDEF os_string_builder_t sb_from_cstr(char *str);
OSSBDEF os_string_builder_t sb_append_cstr(os_string_builder_t sb, char *str);
OSSBDEF os_string_builder_t sb_append_format(os_string_builder_t sb, char *format, ...);
OSSBDEF os_string_builder_t sb_end(os_string_builder_t sb);
OSSBDEF os_string_builder_t sb_reset(os_string_builder_t sb);
OSSBDEF char *sb_as_cstr(os_string_builder_t sb);
OSSBDEF os_string_builder_t sb_free(os_string_builder_t sb);

#ifndef OSSB_REALLOC
#define OSSB_REALLOC realloc
#endif

static void *sb__realloc(void *ptr, size_t size);

#endif

#ifdef OS_SB_IMPLEMENTATION

static void *sb__realloc(void *ptr, size_t size)
{
    return OSSB_REALLOC(ptr, size);
}

OSSBDEF os_string_builder_t sb_new(void)
{
    return (os_string_builder_t) {
        .buffer = NULL,
        .capacity = 0,
        .length = 0,
    };
}

OSSBDEF os_string_builder_t sb_from_cstr(char *str)
{
    os_string_builder_t sb = sb_new();
    sb = sb_append_cstr(sb, str);
    return sb;
}

OSSBDEF os_string_builder_t sb_append_cstr(os_string_builder_t sb, char *str)
{
    size_t length = strlen(str);
    if (sb.length + length >= sb.capacity)
    {
        size_t new_capacity
            = sb.capacity == 0 ? OSSB_INITIAL_CAPACITY : sb.capacity * OSSB_FACTOR;
        while (sb.length + length >= new_capacity)
        {
            new_capacity *= OSSB_FACTOR;
        }
        sb.buffer = sb__realloc(sb.buffer, new_capacity);
        if (NULL == sb.buffer)
        {
            exit(1);
        }
        sb.capacity = new_capacity;
    }

    memcpy(sb.buffer + sb.length, str, length);
    sb.length += length;
    return sb;
}

OSSBDEF os_string_builder_t sb_end(os_string_builder_t sb)
{
    if (sb.length + 1 >= sb.capacity)
    {
        size_t new_capacity
            = sb.capacity == 0 ? OSSB_INITIAL_CAPACITY : sb.capacity * OSSB_FACTOR;
        while (sb.length + 1 >= new_capacity)
        {
            new_capacity *= OSSB_FACTOR;
        }
        sb.buffer = sb__realloc(sb.buffer, new_capacity);
        if (NULL == sb.buffer)
        {
            exit(1);
        }
        sb.capacity = new_capacity;
    }

    sb.buffer[sb.length++] = '\0';
    return sb;
}

OSSBDEF os_string_builder_t sb_reset(os_string_builder_t sb)
{
    sb.length = 0;
    return sb;
}

OSSBDEF char *sb_as_cstr(os_string_builder_t sb)
{
    return sb.buffer;
}

OSSBDEF os_string_builder_t sb_free(os_string_builder_t sb)
{
    if (sb.buffer)
    {
        (void) free(sb.buffer);
        sb.buffer = NULL;
    }

    sb.length = 0;
    sb.capacity = 0;

    return sb;
}
#endif
