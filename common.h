#ifndef nova_common
#define nova_common

/* All needed libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Common macros. */
#define nova_assert(cond, msg, cleanup, ...)        \
    if(cond) {                                      \
        fprintf(stderr, msg, __VA_ARGS__);          \
        cleanup;                                    \
        exit(EXIT_FAILURE);                         \
    }

#define is_ascii(ch)                \
    (ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A)

#define is_number(ch)               \
    ch >= 0x30 && ch <= 0x39

#endif
