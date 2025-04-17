#ifndef API_I2C_H
#define API_I2C_H

#include "API_common.h"

#define I2C_CLOCK_RATE 100000
#define I2C_TIMEOUT 1000

typedef enum {
    I2C_OK = 0,
    I2C_ERROR,         
} I2C_Status_t;

I2C_Status_t I2C_Init(void);
I2C_Status_t I2C_Send(uint8_t address, uint8_t *data, uint16_t size);
I2C_Status_t I2C_Receive(uint8_t address, uint8_t *data, uint16_t size);
I2C_Status_t I2C_isInit(void);


#endif // API_I2C_H
