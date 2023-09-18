#ifndef OSSV_INCLUDE_OS_SV_H
#define OSSV_INCLUDE_OS_SV_H

#include <stdbool.h>
#include <stddef.h>

#ifndef OSSVDEF
#ifdef OSSV_STATIC
#define OSSVDEF static
#else
#define OSSVDEF extern
#endif
#endif

typedef struct
{
    const char *data;
    size_t length;
} os_string_view_t;

#define OSSV_FMT      "%.*s"
#define OSSV_ARG(sv)  (int) (sv).length, (sv).data
#define OSSV_PARG(sv) (int) (sv)->length, (sv)->data

OSSVDEF os_string_view_t ossv_new(const char *data, size_t length);
OSSVDEF os_string_view_t ossv_from_cstr(const char *str);

OSSVDEF os_string_view_t ossv_trim_left(os_string_view_t sv);
OSSVDEF os_string_view_t ossv_trim_right(os_string_view_t sv);
OSSVDEF os_string_view_t ossv_trim(os_string_view_t sv);

OSSVDEF os_string_view_t ossv_chop_by_delimeter(os_string_view_t *sv,
                                                char delimiter);

OSSVDEF bool ossv_equal(os_string_view_t sv, os_string_view_t other);
OSSVDEF bool ossv_equal_cstr(os_string_view_t sv, const char *str);
OSSVDEF bool ossv_starts_with(os_string_view_t sv, os_string_view_t other);
OSSVDEF bool ossv_starts_with_cstr(os_string_view_t sv, const char *str);
OSSVDEF bool ossv_ends_with(os_string_view_t sv, os_string_view_t other);
OSSVDEF bool ossv_ends_with_cstr(os_string_view_t sv, const char *str);

typedef bool (*ossv_predicate)(char character);
OSSVDEF bool ossv_all(os_string_view_t sv, ossv_predicate predicate);
OSSVDEF bool ossv_any(os_string_view_t sv, ossv_predicate predicate);
OSSVDEF bool ossv_is_empty(os_string_view_t sv);
OSSVDEF bool ossv_is_alpha(os_string_view_t sv);
OSSVDEF bool ossv_is_alnum(os_string_view_t sv);
OSSVDEF bool ossv_is_digit(os_string_view_t sv);

#endif

#ifdef OS_SV_IMPLEMENTATION
#include <ctype.h>
#include <string.h>

OSSVDEF os_string_view_t ossv_new(const char *data, size_t length)
{
    return (os_string_view_t) {.data = data, .length = length};
}

OSSVDEF os_string_view_t ossv_from_cstr(const char *str)
{
    return (os_string_view_t) {.data = str, .length = strlen(str)};
}

OSSVDEF os_string_view_t ossv_trim_left(os_string_view_t sv)
{
    while (sv.length && isspace(*sv.data))
    {
        ++sv.data;
        --sv.length;
    }

    return sv;
}

OSSVDEF os_string_view_t ossv_trim_right(os_string_view_t sv)
{
    while (sv.length && isspace(sv.data[sv.length - 1]))
    {
        --sv.length;
    }

    return sv;
}

OSSVDEF os_string_view_t ossv_trim(os_string_view_t sv)
{
    return ossv_trim_right(ossv_trim_left(sv));
}

OSSVDEF os_string_view_t ossv_chop_by_delimeter(os_string_view_t *sv,
                                                char delimiter)
{
    os_string_view_t out = *sv;

    while (sv->length && sv->data[0] != delimiter)
    {
        ++sv->data;
        --sv->length;
    }

    out.length -= sv->length;

    if (sv->length && sv->data[0] == delimiter)
    {
        ++sv->data;
        --sv->length;
    }

    return out;
}

OSSVDEF bool ossv_equal(os_string_view_t sv, os_string_view_t other)
{
    if (sv.length != other.length)
    {
        return false;
    }

    return 0 == strncmp(sv.data, other.data, sv.length);
}

OSSVDEF bool ossv_equal_cstr(os_string_view_t sv, const char *str)
{
    return ossv_equal(sv, ossv_from_cstr(str));
}

OSSVDEF bool ossv_starts_with(os_string_view_t sv, os_string_view_t other)
{
    if (other.length > sv.length)
        return false;

    while (other.length)
    {
        if (*sv.data != *other.data)
            return false;
        ++sv.data;
        ++other.data;
        --sv.length;
        --other.length;
    }

    return true;
}

OSSVDEF bool ossv_starts_with_cstr(os_string_view_t sv, const char *str)
{
    return ossv_starts_with(sv, ossv_from_cstr(str));
}

OSSVDEF bool ossv_ends_with(os_string_view_t sv, os_string_view_t other)
{
    if (other.length > sv.length)
        return false;

    while (other.length)
    {
        if (sv.data[sv.length - 1] != other.data[other.length - 1])
            return false;
        --sv.length;
        --other.length;
    }

    return true;
}

OSSVDEF bool ossv_ends_with_cstr(os_string_view_t sv, const char *str)
{
    return ossv_ends_with(sv, ossv_from_cstr(str));
}

OSSVDEF bool ossv_is_empty(os_string_view_t sv)
{
    return sv.length == 0;
}

OSSVDEF _Bool ossv_all(os_string_view_t sv, ossv_predicate predicate)
{
    for (size_t i = 0; i < sv.length; ++i)
    {
        if (!predicate(sv.data[i]))
        {
            return false;
        }
    }

    return true;
}

OSSVDEF bool ossv_any(os_string_view_t sv, ossv_predicate predicate)
{
    for (size_t i = 0; i < sv.length; ++i)
    {
        if (predicate(sv.data[i]))
        {
            return true;
        }
    }

    return false;
}

#define _retype_predicate(wide_funcname, ctype_funcname)                       \
    static bool wide_funcname(char c)                                          \
    {                                                                          \
        return 0 != ctype_funcname((int) c);                                   \
    }

_retype_predicate(is_alpha, isalpha)
_retype_predicate(is_alnum, isalnum)
_retype_predicate(is_digit, isdigit)

#define _ossv_predicate(ossv_funcname, predicate_funcname)                     \
    OSSVDEF bool ossv_funcname(os_string_view_t sv)                            \
    {                                                                          \
        return ossv_all(sv, (ossv_predicate) predicate_funcname);              \
    }

_ossv_predicate(ossv_is_alpha, is_alpha)
_ossv_predicate(ossv_is_alnum, is_alnum)
_ossv_predicate(ossv_is_digit, is_digit)

#undef _ossv_predicate

#endif
