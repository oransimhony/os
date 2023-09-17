#define SV_IMPLEMENTATION
#include "sv.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    /* Trimming */
    string_view_t sv = sv_from_cstr("      hello world from me   ");
    printf("before: " SV_FMT "\n", SV_ARG(sv));
    sv = sv_trim(sv);
    printf("after: " SV_FMT "\n", SV_ARG(sv));

    /* Starts and ends with */
    assert(sv_starts_with_cstr(sv, "hello"));
    assert(sv_ends_with_cstr(sv, "me"));

    /* Split to words by spaces */
    string_view_t word = sv_chop_by_delimeter(&sv, ' ');
    printf("first: " SV_FMT "\n", SV_ARG(word));
    printf("remaining: " SV_FMT "\n", SV_ARG(sv));
    assert(true == sv_equal_cstr(word, "hello"));
    word = sv_chop_by_delimeter(&sv, ' ');
    printf("second: " SV_FMT "\n", SV_ARG(word));
    printf("remaining: " SV_FMT "\n", SV_ARG(sv));
    word = sv_chop_by_delimeter(&sv, ' ');
    printf("third: " SV_FMT "\n", SV_ARG(word));
    printf("remaining: " SV_FMT "\n", SV_ARG(sv));
    word = sv_chop_by_delimeter(&sv, ' ');
    printf("fourth: " SV_FMT "\n", SV_ARG(word));
    printf("remaining: " SV_FMT "\n", SV_ARG(sv));

    assert(sv_is_empty(sv));

    /* Character class checks */
    sv = sv_from_cstr("hello 123");
    word = sv_chop_by_delimeter(&sv, ' ');
    printf("word: " SV_FMT "\n", SV_ARG(word));
    assert(sv_is_alpha(word));
    assert(sv_is_digit(sv));
}
