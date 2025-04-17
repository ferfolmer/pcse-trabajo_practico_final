#ifndef API_UART_H
#define API_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "API_delay.h"

bool_t uartInit();
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);



#endif // API_UART_H
