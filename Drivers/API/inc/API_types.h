#ifndef API_TYPES_H
#define API_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

typedef uint32_t tick_t;
typedef bool     bool_t;

typedef struct {
    void    *port;
    uint16_t pin;
} gpio_t;

#endif
