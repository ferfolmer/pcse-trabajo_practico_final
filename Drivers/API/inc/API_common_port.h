/**
 * @file API_common_port.h
 * @author Iván Podoroska
 * @brief 
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef COMMON_PORT_H
#define COMMON_PORT_H

#include <stddef.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"

typedef GPIO_PinState   pin_state_t;
typedef uint16_t        pin_t;
typedef GPIO_TypeDef    *port_t;

#endif
