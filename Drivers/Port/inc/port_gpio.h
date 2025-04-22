#ifndef PORT_GPIO_H
#define PORT_GPIO_H

#include <stdint.h>
#include "port_types.h"

pin_state_t Port_ReadPin (port_t port, uint16_t pin);
void        Port_WritePin(port_t port, uint16_t pin, pin_state_t state);


#endif /* PORT_GPIO_H */
