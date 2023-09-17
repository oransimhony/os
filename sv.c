#include "sv.h"
#include <ctype.h>
#include <string.h>

string_view_t sv_new(char *data, size_t length)
{
    return (string_view_t) {.data = data, .length = length};
}

string_view_t sv_from_cstr(char *str)
{
    return (string_view_t) {.data = str, .length = strlen(str)};
}

string_view_t sv_trim_left(string_view_t sv)
{
    while (sv.length && isspace(*sv.data))
    {
        ++sv.data;
        --sv.length;
    }

    return sv;
}

string_view_t sv_trim_right(string_view_t sv)
{
    while (sv.length && isspace(sv.data[sv.length - 1]))
    {
        --sv.length;
    }

    return sv;
}

string_view_t sv_trim(string_view_t sv)
{
    return sv_trim_right(sv_trim_left(sv));
}

string_view_t sv_chop_by_delimeter(string_view_t *sv, char delimiter)
{
    string_view_t out = *sv;

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

bool sv_equal(string_view_t sv, string_view_t other)
{
    if (sv.length != other.length)
    {
        return false;
    }

    return 0 == strncmp(sv.data, other.data, sv.length);
}

bool sv_equal_cstr(string_view_t sv, char *str)
{
    return sv_equal(sv, sv_from_cstr(str));
}

bool sv_starts_with(string_view_t sv, string_view_t other)
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

bool sv_starts_with_cstr(string_view_t sv, char *str)
{
    return sv_starts_with(sv, sv_from_cstr(str));
}

bool sv_ends_with(string_view_t sv, string_view_t other)
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

bool sv_ends_with_cstr(string_view_t sv, char *str)
{
    return sv_ends_with(sv, sv_from_cstr(str));
}

bool sv_is_empty(string_view_t sv)
{
    return sv.length == 0;
}

_Bool sv_all(string_view_t sv, sv_predicate predicate)
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

bool sv_any(string_view_t sv, sv_predicate predicate)
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

#define _sv_predicate(sv_funcname, predicate_funcname)                         \
    bool sv_funcname(string_view_t sv)                                         \
    {                                                                          \
        return sv_all(sv, (sv_predicate) predicate_funcname);                  \
    }

_sv_predicate(sv_is_alpha, is_alpha)
_sv_predicate(sv_is_alnum, is_alnum)
_sv_predicate(sv_is_digit, is_digit)

#undef _sv_predicate
