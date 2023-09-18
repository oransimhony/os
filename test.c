#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define OS_LOG_IMPLEMENTATION
#include "os_log.h"
#define OS_SB_IMPLEMENTATION
#include "os_sb.h"
#define OS_SV_IMPLEMENTATION
#include "os_sv.h"

/* Log tests */
int check_log(void)
{
    printf("------------Checking Log-----------\n");
    osl_log(OSL_DEBUG, "debug log");
    osl_log(OSL_INFO, "info log");
    osl_log(OSL_WARNING, "warning log");
    osl_log(OSL_ERROR, "error log");
    osl_logf(OSL_WARNING, "sizeof(size_t) = %zu", sizeof(size_t));
    printf("------------Done checking Log-----------\n");
    return 1;
}

/* SB tests */
int check_sb(void)
{
    printf("------------Checking SB-----------\n");
    os_string_builder_t sb = sb_new();
    assert(sb.capacity == 0);
    sb = sb_append_cstr(sb, "hello");
    assert(sb.length == strlen("hello"));
    assert(sb.capacity != 0);
    sb = sb_append_cstr(sb, " ");
    assert(sb.length == strlen("hello "));
    sb = sb_append_cstr(sb, "world!!");
    assert(sb.length == strlen("hello world!!"));
    sb = sb_end(sb);
    assert(sb.length == strlen("hello world!!") + 1);
    printf("buffer(%zu): %s\n", sb.capacity, sb_as_cstr(sb));

    sb = sb_reset(sb);
    assert(sb.capacity != 0);

    sb = sb_append_cstr(sb, "hello again...");
    assert(sb.length == strlen("hello again..."));
    sb = sb_end(sb);
    assert(sb.length == strlen("hello again...") + 1);
    printf("buffer(%zu): %s\n", sb.capacity, sb_as_cstr(sb));
    sb = sb_free(sb);
    assert(sb.capacity == 0);

    sb = sb_from_cstr("hello for the last time");
    assert(sb.capacity != 0);
    assert(sb.length == strlen("hello for the last time"));
    sb = sb_end(sb);
    printf("buffer(%zu): %s\n", sb.capacity, sb_as_cstr(sb));
    sb = sb_free(sb);
    assert(sb.capacity == 0);
    printf("------------Done checking SB-----------\n");

    return 1;
}

/* SV tests */
int check_sv(void)
{
    printf("------------Checking SV-----------\n");
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
    printf("------------Done checking SV-----------\n");

    return 1;
}

int main(void)
{
    if (!check_log())
    {
        (void) fprintf(stderr, "Log implementation failed");
        return EXIT_FAILURE;
    }

    if (!check_sb())
    {
        (void) fprintf(stderr, "SB implementation failed");
        return EXIT_FAILURE;
    }

    if (!check_sv())
    {
        (void) fprintf(stderr, "SV implementation failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
