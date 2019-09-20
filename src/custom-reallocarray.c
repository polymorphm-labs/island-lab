#define _GNU_SOURCE

// *alloc*
#include <stdlib.h>

#define S(name) island_lab_custom_reallocarray_ ## name

void *
S (reallocarray) (void *ptr, size_t nmemb, size_t size)
{
    // we could use __builtin_mul_overflow() for overflow protecting
    // but if you had to use custom reallocarray() implementation
    // there is a high probability this builtin-function is absent too

    size_t mul_size = nmemb * size;

    if (mul_size < size || mul_size < nmemb)
    {
        return 0;
    }

    return realloc (ptr, mul_size);
}

// vi:ts=4:sw=4:et
