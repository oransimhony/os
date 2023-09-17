#ifndef __SV_H__
#define __SV_H__

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    char *data;
    size_t length;
} string_view_t;

#define SV_FMT     "%.*s"
#define SV_ARG(sv) (int) sv.length, sv.data

string_view_t sv_new(char *data, size_t length);
string_view_t sv_from_cstr(char *str);

string_view_t sv_trim_left(string_view_t sv);
string_view_t sv_trim_right(string_view_t sv);
string_view_t sv_trim(string_view_t sv);

string_view_t sv_chop_by_delimeter(string_view_t *sv, char delimiter);

bool sv_equal(string_view_t sv, string_view_t other);
bool sv_equal_cstr(string_view_t sv, char *str);
bool sv_starts_with(string_view_t sv, string_view_t other);
bool sv_starts_with_cstr(string_view_t sv, char *str);
bool sv_ends_with(string_view_t sv, string_view_t other);
bool sv_ends_with_cstr(string_view_t sv, char *str);

typedef bool (*sv_predicate)(char character);
bool sv_all(string_view_t sv, sv_predicate predicate);
bool sv_any(string_view_t sv, sv_predicate predicate);
bool sv_is_empty(string_view_t sv);
bool sv_is_alpha(string_view_t sv);
bool sv_is_alnum(string_view_t sv);
bool sv_is_digit(string_view_t sv);

#endif
