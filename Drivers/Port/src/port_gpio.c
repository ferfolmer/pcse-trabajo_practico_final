#include "port_gpio.h"
#include "port_types.h"

pin_state_t Port_ReadPin(port_t port, uint16_t pin)
{
    return HAL_GPIO_ReadPin(port, pin);
}

void Port_WritePin(port_t port, uint16_t pin, pin_state_t st)
{
    HAL_GPIO_WritePin(port, pin, st);
}
