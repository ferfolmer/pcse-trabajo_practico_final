#include "API_common_port.h"

void Port_Delay(uint32_t delayTime)
{
    HAL_Delay(delayTime);
}

pin_state_t Port_ReadPin(port_t port, pin_t pin)
{
    return HAL_GPIO_ReadPin(port, pin);
}
