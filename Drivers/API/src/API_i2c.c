#include "API_i2c.h"
#include "stm32f4xx_hal.h"

#define I2C_INSTANCE I2C1
#define I2C_CLOCK_RATE 100000
#define I2C_TIMEOUT 1000

static I2C_HandleTypeDef hi2c1;

static bool_t isInit_ = false;

static void I2C_GPIO_Init(I2C_HandleTypeDef *hi2c);
static void Error_Handler(void);


/**
 * @brief  Inicializa la instancia de I2C utilizada para la comunicacion con los perifericos 
 * 
 * @return I2C_Status_t I2C_OK si la inicializacion fue exitosa, I2C_ERROR en caso contrario
 */
I2C_Status_t I2C_Init(void)
{
    if (isInit_) 
    {
        return I2C_OK;
    }

    hi2c1.Instance = I2C_INSTANCE;
    hi2c1.Init.ClockSpeed = I2C_CLOCK_RATE;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    I2C_GPIO_Init(&hi2c1);

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
      Error_Handler();
      return I2C_ERROR;
    }

    isInit_ = true;
    return I2C_OK;
}

/**
 * @brief envia datos a un periferico I2C
 * 
 * @param address direccion del dispositivo I2C
 * @param data puntero a los datos a enviar
 * @param size 
 * @return I2C_Status_t si la transferencia fue exitosa devuelve I2C_OK, en caso contrario I2C_ERROR
 */
I2C_Status_t I2C_Send(uint8_t address, uint8_t *data, uint16_t size)
{
    if (HAL_I2C_Master_Transmit(&hi2c1, address, data, size, I2C_TIMEOUT) != HAL_OK)
    {
        return I2C_ERROR;
    }
    return I2C_OK;
}

/**
 * @brief recibe datos de un periferico I2C
 * 
 * @param address direccion del dispositivo I2C
 * @param data puntero a los datos a recibir
 * @param size 
 * @return I2C_Status_t si la transferencia fue exitosa devuelve I2C_OK, en caso contrario I2C_ERROR
 */
I2C_Status_t I2C_Receive(uint8_t address, uint8_t *data, uint16_t size)
{
    if (HAL_I2C_Master_Receive(&hi2c1, address, data, size, I2C_TIMEOUT) != HAL_OK)
    {
        return I2C_ERROR;
    }
    return I2C_OK;
}

/**
 * @brief 
 * 
 * @return I2C_Status_t I2C_OK si la instancia de I2C fue inicializada, I2C_ERROR en caso contrario
 */
I2C_Status_t I2C_isInit(void)
{
    return isInit_ ? I2C_OK : I2C_ERROR;
}


static void I2C_GPIO_Init(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance == I2C1)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_I2C1_CLK_ENABLE();
  }
}

/**
 * @brief Error handler function that is called when an invalid operation occurs
 * 
 */
static void Error_Handler(void)
{
    while(1)
    {}
}
