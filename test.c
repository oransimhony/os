#include <stdlib.h>
#define OS_SV_IMPLEMENTATION
#include "os_sv.h"
#include <assert.h>
#include <stdio.h>

int check_sv(void)
{
    /* Trimming */
    os_string_view_t sv = ossv_from_cstr("      hello world from me   ");
    printf("before: " OSSV_FMT "\n", OSSV_ARG(sv));
    sv = ossv_trim(sv);
    printf("after: " OSSV_FMT "\n", OSSV_ARG(sv));

    /* Starts and ends with */
    assert(ossv_starts_with_cstr(sv, "hello"));
    assert(ossv_ends_with_cstr(sv, "me"));

    /* Split to words by spaces */
    os_string_view_t word = ossv_chop_by_delimeter(&sv, ' ');
    printf("first: " OSSV_FMT "\n", OSSV_ARG(word));
    printf("remaining: " OSSV_FMT "\n", OSSV_ARG(sv));
    assert(true == ossv_equal_cstr(word, "hello"));
    word = ossv_chop_by_delimeter(&sv, ' ');
    printf("second: " OSSV_FMT "\n", OSSV_ARG(word));
    printf("remaining: " OSSV_FMT "\n", OSSV_ARG(sv));
    word = ossv_chop_by_delimeter(&sv, ' ');
    printf("third: " OSSV_FMT "\n", OSSV_ARG(word));
    printf("remaining: " OSSV_FMT "\n", OSSV_ARG(sv));
    word = ossv_chop_by_delimeter(&sv, ' ');
    printf("fourth: " OSSV_FMT "\n", OSSV_ARG(word));
    printf("remaining: " OSSV_FMT "\n", OSSV_ARG(sv));

    assert(ossv_is_empty(sv));

    /* Character class checks */
    sv = ossv_from_cstr("hello 123");
    word = ossv_chop_by_delimeter(&sv, ' ');
    printf("word: " OSSV_FMT "\n", OSSV_ARG(word));
    assert(ossv_is_alpha(word));
    assert(ossv_is_digit(sv));

    return 1;
}

int main(void)
{
    if (!check_sv())
    {
      (void)fprintf(stderr, "SV implementation failed");
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
