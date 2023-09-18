#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define OS_DA_IMPLEMENTATION
#include "os_da.h"
#define OS_LOG_IMPLEMENTATION
#include "os_log.h"
#define OS_SB_IMPLEMENTATION
#include "os_sb.h"
#define OS_SV_IMPLEMENTATION
#include "os_sv.h"

// Headers with other headers as dependencies
#define OS_JSON_IMPLEMENTATION
#include "os_json.h"

#define CHECK_START(name) printf("------------Checking %s -----------\n", name)
#define CHECK_END(name)                                                        \
    printf("------------Done checking %s-----------\n", name)

#define _stringify(x) #x
#define stringify(x)  _stringify(x)
#define TEST_STAGE(check_func, name)                                           \
    do                                                                         \
    {                                                                          \
        if (!check_func(#name))                                                \
        {                                                                      \
            fprintf(stderr, __FILE__                                           \
                    ":" stringify(__LINE__) ":ERROR:" #name                    \
                                            " implementation failed\n");       \
            return EXIT_FAILURE;                                               \
        }                                                                      \
    } while (0)

/* DA tests */
int check_da(const char *stage)
{
    CHECK_START(stage);

    typedef struct
    {
        int *items;
        size_t count;
        size_t capacity;
    } os_numbers_t;

    os_numbers_t numbers = {0};
    assert(numbers.capacity == 0);
    assert(numbers.count == 0);

    /* Appending works */
    os_da_append(numbers, 1);
    assert(numbers.count == 1);
    assert(numbers.items[0] == 1);
    os_da_append(numbers, 2);
    assert(numbers.count == 2);
    assert(numbers.items[1] == 2);
    os_da_append(numbers, 3);
    assert(numbers.count == 3);
    assert(numbers.items[2] == 3);

    /* Pop from the end */
    os_da_pop_tail(numbers);
    assert(numbers.count == 2);
    assert(numbers.items[0] == 1);

    /* Pop from the start */
    os_da_pop_head(int, numbers);
    assert(numbers.count == 1);
    assert(numbers.items[0] == 2);

    os_da_pop_head(int, numbers);
    assert(numbers.count == 0);

    /* Can pop empty DAs */
    os_da_pop_head(int, numbers);
    assert(numbers.count == 0);

    os_da_free(numbers);

    CHECK_END(stage);
    return 1;
}

/* JSON tests */
int check_json(const char *stage)
{
    CHECK_START(stage);

    os_json_object_t *obj = osj_parse("{\"hello\": \"world\",\"number\": 1, \"isWorking\": true}");
    assert(obj->keys.count == 3);
    assert(obj->values.count == 3);

    assert(0 == strcmp("hello", obj->keys.items[0]));
    assert(0 == strcmp("world", obj->values.items[0]->value.str));

    assert(0 == strcmp("number", obj->keys.items[1]));
    assert(1.0L == obj->values.items[1]->value.number);

    assert(0 == strcmp("isWorking", obj->keys.items[2]));
    assert(true == obj->values.items[2]->value.boolean);

    osj_free(obj);

    obj = osj_parse("{}");
    assert(obj->keys.count == 0);
    assert(obj->values.count == 0);

    osj_free(obj);

    obj = osj_parse("{\"a\": \"b\"}");
    assert(obj->keys.count == 1);
    assert(obj->values.count == 1);

    assert(0 == strcmp("a", obj->keys.items[0]));
    assert(0 == strcmp("b", obj->values.items[0]->value.str));

    osj_free(obj);

    obj = osj_parse("{\"a\": {\"b\": \"c\"}}");
    assert(obj->keys.count == 1);
    assert(obj->values.count == 1);

    assert(0 == strcmp("a", obj->keys.items[0]));
    assert(1 == obj->values.items[0]->value.object->keys.count);
    assert(1 == obj->values.items[0]->value.object->values.count);
    assert(0 == strcmp("b", obj->values.items[0]->value.object->keys.items[0]));
    assert(0 == strcmp("c", obj->values.items[0]->value.object->values.items[0]->value.str));

    osj_free(obj);

    CHECK_END(stage);
    return 1;
}

/* Log tests */
int check_log(const char *stage)
{
    CHECK_START(stage);
    osl_log(OSL_DEBUG, "debug log");
    osl_log(OSL_INFO, "info log");
    osl_log(OSL_WARNING, "warning log");
    osl_log(OSL_ERROR, "error log");
    osl_logf(OSL_WARNING, "sizeof(size_t) = %zu", sizeof(size_t));
    CHECK_END(stage);
    return 1;
}

/* SB tests */
int check_sb(const char *stage)
{
    CHECK_START(stage);
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
    CHECK_END(stage);

    return 1;
}

/* SV tests */
int check_sv(const char *stage)
{
    CHECK_START(stage);
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
    CHECK_END(stage);

    return 1;
}

int main(void)
{
    TEST_STAGE(check_da, DA);
    TEST_STAGE(check_json, JSON);
    TEST_STAGE(check_log, Log);
    TEST_STAGE(check_sb, SB);
    TEST_STAGE(check_sv, SV);

    return EXIT_SUCCESS;
}
