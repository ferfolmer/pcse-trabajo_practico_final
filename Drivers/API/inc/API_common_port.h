#ifndef API_COMMON_PORT_H
#define API_COMMON_PORT_H

#include <stddef.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"

typedef GPIO_PinState   pin_state_t;
typedef uint16_t        pin_t;
typedef GPIO_TypeDef    *port_t;

void Port_Delay(uint32_t delayTime);
pin_state_t Port_ReadPin(port_t GPIOx, pin_t GPIO_Pin);

#endif
