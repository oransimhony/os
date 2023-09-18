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

/*
Dynamic Array Type template:
typedef struct
{
    void *items;
    size_t count;
    size_t capacity;
} os_dynamic_array_t;
 */

#ifndef OSDA_REALLOC
#define OSDA_REALLOC realloc
#endif

#ifndef OSDA_FREE
#define OSDA_FREE free
#endif

#define os_da_free(da)                                                         \
    do                                                                         \
    {                                                                          \
        if ((da).items)                                                        \
        {                                                                      \
            da__free((da).items);                                              \
            (da).items = NULL;                                                 \
        }                                                                      \
        (da).count = 0;                                                        \
        (da).capacity = 0;                                                     \
    } while (0)

static void *da__realloc(void *ptr, size_t size);
static void da__free(void *ptr);

#define os_da_append(da, item)                                                 \
    do                                                                         \
    {                                                                          \
        if ((da).count >= (da).capacity)                                       \
        {                                                                      \
            size_t new_capacity = (da).capacity == 0                           \
                                    ? OSDA_INITIAL_CAPACITY                    \
                                    : (da).capacity * OSDA_FACTOR;             \
            while ((da).count >= new_capacity)                                 \
            {                                                                  \
                (da).capacity *= OSDA_FACTOR;                                  \
            }                                                                  \
            (da).items                                                         \
                = da__realloc((da).items, new_capacity * sizeof(*(da).items)); \
            (da).capacity = new_capacity;                                      \
        }                                                                      \
        (da).items[(da).count++] = item;                                       \
    } while (0)

#define os_da_pop_tail(da)                                                     \
    do                                                                         \
    {                                                                          \
        if ((da).count > 0)                                                    \
            --(da).count;                                                      \
    } while (0)

#define os_da_pop_head(type, da)                                               \
    do                                                                         \
    {                                                                          \
        if ((da).count <= 1)                                                   \
        {                                                                      \
            os_da_pop_tail((da));                                              \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            memmove((da).items, (type *) (da).items + 1, sizeof(type));        \
            --(da).count;                                                      \
        }                                                                      \
    } while (0)

#endif

#ifdef OS_DA_IMPLEMENTATION

static void *da__realloc(void *ptr, size_t size)
{
    return OSDA_REALLOC(ptr, size);
}

static void da__free(void *ptr)
{
    OSDA_FREE(ptr);
}

#endif
