#ifndef OSJ_INCLUDE_OS_JSON_H
#define OSJ_INCLUDE_OS_JSON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef OSDA_INCLUDE_OS_DA_H
#include "os_da.h"
#endif

#ifndef OSJDEF
#ifdef OSJ_STATIC
#define OSJDEF static
#else
#define OSJDEF extern
#endif
#endif

typedef enum
{
    osj_number,
    osj_str,
    osj_boolean,
    osj_null,
    osj_object,
    osj_array,
    osj_value_type_count,
} os_json_value_type_t;

static const char *osj_value_type_to_cstr[osj_value_type_count] = {
    [osj_number] = "number", [osj_str] = "string",    [osj_boolean] = "bool",
    [osj_null] = "null",     [osj_object] = "object", [osj_array] = "array",
};

struct os_json_object_s;
struct os_json_values_s;

typedef union
{
    double number;
    char *str;
    bool boolean;
    struct os_json_object_s *object;
    struct os_json_values_s *array;
} os_json_inner_value_t;

typedef struct
{
    os_json_value_type_t type;
    os_json_inner_value_t value;
} os_json_value_t;

typedef struct
{
    char **items;
    size_t count;
    size_t capacity;
} os_json_keys_t;

typedef struct os_json_values_s
{
    os_json_value_t **items;
    size_t count;
    size_t capacity;
} os_json_values_t;

typedef struct os_json_object_s
{
    os_json_keys_t keys;
    os_json_values_t values;
} os_json_object_t;

OSJDEF os_json_object_t *osj_parse(const char *str);
OSJDEF void osj_print(os_json_object_t obj);
OSJDEF void osj_free(os_json_object_t *obj);

#ifndef OSJ_REALLOC
#define OSJ_REALLOC realloc
#endif

#ifndef OSJ_FREE
#define OSJ_FREE free
#endif

static void *osj__realloc(void *ptr, size_t size);
static void osj__free(void *ptr);

#endif

#ifdef OS_JSON_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

#ifdef OSJ_LOGGING
#ifndef OS_LOG_IMPLEMENTATION
#define OS_LOG_IMPLEMENTATION
#include "os_log.h"
#endif
#else
#define osl_log(...)
#define osl_logf(...)
#endif

#ifndef OS_DA_IMPLEMENTATION
#define OS_DA_IMPLEMENTATION
#include "os_da.h"
#endif

#ifndef OS_SB_IMPLEMENTATION
#define OS_SB_IMPLEMENTATION
#include "os_sb.h"
#endif

#ifndef OS_SV_IMPLEMENTATION
#define OS_SV_IMPLEMENTATION
#include "os_sv.h"
#endif

static os_json_value_t *osj__parse_value(os_string_view_t *sv);
static os_json_object_t *osj__parse_object(os_string_view_t *sv);
static void osj__print_object(os_json_object_t obj);

static void *osj__realloc(void *ptr, size_t size)
{
    return OSJ_REALLOC(ptr, size);
}

static void osj__free(void *ptr)
{
    OSJ_FREE(ptr);
}

static char *osj__sv_as_cstr(os_string_view_t sv)
{
    char *str = (char *) osj__realloc(NULL, sv.length + 1);
    assert(str != NULL);
    memcpy(str, sv.data, sv.length);
    str[sv.length] = '\0';
    return str;
}

static os_json_value_t *osj__parse_string(os_string_view_t *sv)
{
    ossv_chop_by_delimeter(sv, '"');
    os_string_view_t str = ossv_chop_by_delimeter(sv, '"');
    os_json_value_t *val = (os_json_value_t *) osj__realloc(NULL, sizeof(*val));
    assert(val != NULL);

    val->type = osj_str;
    val->value.str = osj__sv_as_cstr(str);

    return val;
}

static os_json_value_t *osj__parse_number(os_string_view_t *sv)
{
    os_json_value_t *val = (os_json_value_t *) osj__realloc(NULL, sizeof(*val));
    assert(val != NULL);

    char *endptr = NULL;

    val->type = osj_number;
    val->value.number = strtod(sv->data, &endptr);

    size_t offset = endptr - sv->data;
    sv->data += offset;
    sv->length -= offset;

    return val;
}

static os_json_value_t *osj__parse_literal(os_string_view_t *sv)
{
    os_json_value_t *val = (os_json_value_t *) osj__realloc(NULL, sizeof(*val));
    assert(val != NULL);

    os_string_view_t literal = ossv_new(sv->data, 0);
    while (isalpha(*sv->data))
    {
        ++sv->data;
        --sv->length;
        ++literal.length;
    }
    ossv_trim(literal);
    if (ossv_equal_cstr(literal, "true"))
    {
        val->type = osj_boolean;
        val->value.boolean = true;
    }
    else if (ossv_equal_cstr(literal, "false"))
    {
        val->type = osj_boolean;
        val->value.boolean = false;
    }
    else if (ossv_equal_cstr(literal, "null"))
    {
        val->type = osj_null;
    }
    else
    {
        osl_logf(OSL_ERROR, "Illegal value %s for literal", literal);
        return NULL;
    }

    return val;
}

static os_json_value_t *osj__parse_array(os_string_view_t *sv)
{
    os_json_value_t *val = (os_json_value_t *) osj__realloc(NULL, sizeof(*val));
    assert(val != NULL);

    assert(*sv->data == '[');
    ++sv->data;
    --sv->length;
    *sv = ossv_trim_left(*sv);

    os_json_values_t *array
        = (os_json_values_t *) osj__realloc(NULL, sizeof(*array));
    assert(array != NULL);
    array->items = NULL;
    array->count = 0;
    array->capacity = 0;

    while (*sv->data != ']')
    {
        os_da_append(array, osj__parse_value(sv));
        *sv = ossv_trim_left(*sv);
        if (*sv->data == ',')
        {
            ++sv->data;
            --sv->length;
        }
        *sv = ossv_trim_left(*sv);
    }

    ++sv->data;
    --sv->length;

    val->type = osj_array;
    val->value.array = array;

    return val;
}
static os_json_value_t *osj__parse_value(os_string_view_t *sv)
{
    switch (*sv->data)
    {
        case '"':
            return osj__parse_string(sv);
        case '{':
            {
                os_json_object_t *obj = osj__parse_object(sv);
                os_json_value_t *val
                    = (os_json_value_t *) osj__realloc(NULL, sizeof(*val));
                assert(val != NULL);

                val->type = osj_object;
                val->value.object = obj;
                return val;
            }
        case '[':
            return osj__parse_array(sv);
        default:
            if (isdigit(*sv->data))
                return osj__parse_number(sv);

            return osj__parse_literal(sv);

            return NULL;
    }
}

static void osj__parse_entity(os_json_object_t *obj, os_string_view_t *sv)
{
    ossv_chop_by_delimeter(sv, '"');
    os_string_view_t field = ossv_chop_by_delimeter(sv, '"');
    osl_logf(OSL_DEBUG, "field name: " OSSV_FMT, OSSV_ARG(field));

    os_da_append(&obj->keys, osj__sv_as_cstr(field));

    ossv_chop_by_delimeter(sv, ':');
    *sv = ossv_trim_left(*sv);
    os_json_value_t *value = osj__parse_value(sv);
    osl_logf(OSL_DEBUG, "value type %s (%d)",
             osj_value_type_to_cstr[value->type], value->type);

    switch (value->type)
    {
        case osj_str:
            osl_logf(OSL_DEBUG, "value is `%s`", value->value.str);
            break;
        case osj_number:
            osl_logf(OSL_DEBUG, "value is %lf", value->value.number);
            break;
        case osj_boolean:
            osl_logf(OSL_DEBUG, "value is %s",
                     value->value.boolean ? "true" : "false");
            break;
        default:
            break;
    }

    os_da_append(&obj->values, value);

    osl_logf(OSL_DEBUG, "remaining: " OSSV_FMT, OSSV_PARG(sv));
}

static os_json_object_t *osj__parse_object(os_string_view_t *sv)
{
    os_json_object_t *obj
        = (os_json_object_t *) osj__realloc(NULL, sizeof(*obj));
    assert(obj != NULL);
    obj->keys = (os_json_keys_t) {0};
    obj->values = (os_json_values_t) {0};

    if (!sv->length)
        goto end;

    if (sv->data[0] != '{')
        goto end;

    ++sv->data;
    --sv->length;
    if (!sv->length || sv->data[0] == '}')
        goto end;

    osj__parse_entity(obj, sv);
    *sv = ossv_trim_left(*sv);
    while (*sv->data == ',')
    {
        ossv_chop_by_delimeter(sv, ',');
        osj__parse_entity(obj, sv);
    }

    assert(*sv->data == '}');
    ++sv->data;
    --sv->length;

end:
    return obj;
}

OSJDEF os_json_object_t *osj_parse(const char *str)
{
    osl_logf(OSL_DEBUG, "String to parse: %s", str);
    os_string_view_t sv = ossv_from_cstr(str);

    ossv_trim(sv);

    os_json_object_t *obj = osj__parse_object(&sv);

    /* Make sure the string is exhausted */
    os_string_view_t end = ossv_chop_by_delimeter(&sv, '}');
    end = ossv_trim(end);
    sv = ossv_trim(sv);
    assert(ossv_equal(end, sv));

    return obj;
}

static void osj__print_value(os_json_value_t *value)
{
    switch (value->type)
    {
        case osj_str:
            printf("\"%s\"", value->value.str);
            break;
        case osj_boolean:
            printf("%s", value->value.boolean ? "true" : "false");
            break;
        case osj_null:
            printf("null");
            break;
        case osj_number:
            printf("%.10lf", value->value.number);
            break;
        case osj_object:
            osj__print_object(*value->value.object);
            break;
        case osj_array:
            printf("[");
            for (size_t j = 0; j < value->value.array->count - 1; ++j)
            {
                osj__print_value(value->value.array->items[j]);
                printf(", ");
            }
            osj__print_value(
                value->value.array->items[value->value.array->count - 1]);
            printf("]");
            break;
        case osj_value_type_count:
            break;
    }
}

static void osj__print_object(os_json_object_t obj)
{
    assert(obj.values.count == obj.keys.count);
    printf("{");

    for (size_t i = 0; i < obj.keys.count - 1; ++i)
    {
        printf("\"%s\": ", obj.keys.items[i]);
        osj__print_value(obj.values.items[i]);
        printf(", ");
    }
    printf("\"%s\": ", obj.keys.items[obj.keys.count - 1]);
    osj__print_value(obj.values.items[obj.values.count - 1]);

    printf("}");
}

OSJDEF void osj_print(os_json_object_t obj)
{
    osj__print_object(obj);
    printf("\n");
}

static void osj__free_value(os_json_value_t *value)
{
    if (value->type == osj_str)
    {
        osj__free(value->value.str);
    }
    else if (value->type == osj_object)
    {
        osj_free(value->value.object);
    }
    else if (value->type == osj_array)
    {
        for (size_t j = 0; j < value->value.array->count; ++j)
        {
            osj__free_value(value->value.array->items[j]);
        }

        os_da_free(value->value.array);
        osj__free(value->value.array);
    }
    osj__free(value);
}

OSJDEF void osj_free(os_json_object_t *obj)
{
    if (!obj)
        return;

    for (size_t i = 0; i < obj->keys.count; ++i)
    {
        osj__free(obj->keys.items[i]);
    }
    os_da_free(&obj->keys);

    for (size_t i = 0; i < obj->values.count; ++i)
    {
        osj__free_value(obj->values.items[i]);
    }
    os_da_free(&obj->values);

    osj__free(obj);
}
#endif
