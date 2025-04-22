#ifndef PORT_H
#define PORT_H

#include "API_common.h"

typedef enum {
    PORT_OK = 0,
    PORT_ERROR
} PortStatus_t;

/* ---------- I²C --------- */
PortStatus_t Port_I2C_Init(void);
PortStatus_t Port_I2C_Send   (uint16_t addr, const uint8_t *buf, uint16_t len);
PortStatus_t Port_I2C_Receive(uint16_t addr,       uint8_t *buf, uint16_t len);

/* ---------- UART -------- */
PortStatus_t Port_UART_Init(void);
PortStatus_t Port_UART_Send(const uint8_t *buf, uint16_t len);
PortStatus_t Port_UART_Receive(uint8_t *buf, uint16_t len);

/* ---------- Encoder -------- */
PortStatus_t Port_Encoder_InitPins(GPIO_TypeDef *clkPort,uint16_t clkPin,
								   GPIO_TypeDef *dtPort	,uint16_t dtPin	,
								   GPIO_TypeDef *swPort	,uint16_t swPin );


#endif
