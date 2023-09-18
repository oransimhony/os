#ifndef OSDA_INCLUDE_OS_DA_H
#define OSDA_INCLUDE_OS_DA_H

#include <stddef.h>
#include <stdlib.h>

#ifndef OSDADEF
#ifdef OSDA_STATIC
#define OSDADEF static
#else
#define OSDADEF extern
#endif
#endif

#ifndef OSDA_INITIAL_CAPACITY
#define OSDA_INITIAL_CAPACITY (8)
#endif

#ifndef OSDA_FACTOR
#define OSDA_FACTOR (2)
#endif

typedef struct
{
    void *items;
    size_t count;
    size_t capacity;
} os_dynamic_array_t;

#ifndef OSDA_REALLOC
#define OSDA_REALLOC realloc
#endif

OSDADEF os_dynamic_array_t os_da_new(void);
OSDADEF os_dynamic_array_t os_da_pop_tail(os_dynamic_array_t da);
OSDADEF os_dynamic_array_t os_da_free(os_dynamic_array_t da);

static void *da__realloc(void *ptr, size_t size);

#define os_da_append(type, da, item)                                           \
    do                                                                         \
    {                                                                          \
        if (da.count + 1 >= da.capacity)                                       \
        {                                                                      \
            size_t new_capacity = da.capacity == 0                             \
                                    ? OSDA_INITIAL_CAPACITY                    \
                                    : da.capacity * OSDA_FACTOR;               \
            da.items = da__realloc(da.items, new_capacity);                    \
            da.capacity = new_capacity;                                        \
        }                                                                      \
        memcpy((type *) da.items + da.count, &(type) {item}, sizeof(type));    \
        ++da.count;                                                            \
    } while (0)

#define os_da_get(type, da, index) ((type *)(da).items)[(index)]

#define os_da_pop_head(type, da)                                               \
    do                                                                         \
    {                                                                          \
        if (da.count <= 1)                                                     \
        {                                                                      \
            da = os_da_pop_tail(da);                                           \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            memmove(da.items, (type *) da.items + 1, sizeof(type));            \
            --da.count;                                                        \
        }                                                                      \
    } while (0)

#endif

#ifdef OS_DA_IMPLEMENTATION

static void *da__realloc(void *ptr, size_t size)
{
    return OSDA_REALLOC(ptr, size);
}

OSDADEF os_dynamic_array_t os_da_new(void)
{
    return (os_dynamic_array_t) {
        .items = NULL,
        .count = 0,
        .capacity = 0,
    };
}

OSDADEF os_dynamic_array_t os_da_pop_tail(os_dynamic_array_t da)
{
    if (da.count > 0)
        --da.count;

    return da;
}

OSDADEF os_dynamic_array_t os_da_free(os_dynamic_array_t da)
{
    if (da.items)
    {
        da__realloc(da.items, 0);
        da.items = NULL;
    }

    da.count = 0;
    da.capacity = 0;
    return da;
}

#endif
