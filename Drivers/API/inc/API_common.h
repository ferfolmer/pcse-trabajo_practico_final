#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "API_common_port.h"

typedef uint32_t tick_t;
typedef bool     bool_t;

typedef struct
{
    port_t  port;
    pin_t   pin;
} gpio_t;

#endif