#include "API_i2c.h"
#include "stm32f4xx_hal.h"

static I2C_HandleTypeDef hi2c1;

static bool_t isInit_ = false;

I2C_Status_t I2C_Init(void)
{
    if (isInit_) 
    {
        return I2C_OK;
    }

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = I2C_CLOCK_RATE;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
      Error_Handler();
    }
}
