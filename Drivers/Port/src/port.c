#include "port.h"
#include "stm32f4xx_hal.h"

/* ===== I²C1 =================================================== */
static I2C_HandleTypeDef hi2c1;

#define I2C_CLOCK_FREQ 100000
#define I2C_TIMEOUT 100

PortStatus_t Port_I2C_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef g = {
        .Pin       = GPIO_PIN_6 | GPIO_PIN_9,
        .Mode      = GPIO_MODE_AF_OD,
        .Pull      = GPIO_PULLUP,
        .Speed     = GPIO_SPEED_FREQ_LOW,
        .Alternate = GPIO_AF4_I2C1
    };
    HAL_GPIO_Init(GPIOB, &g);

    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = I2C_CLOCK_FREQ;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.OwnAddress2     = 0;

    return (HAL_I2C_Init(&hi2c1) == HAL_OK) ? PORT_OK : PORT_ERROR;
}

PortStatus_t Port_I2C_Send(uint16_t addr,const uint8_t *b,uint16_t n)
{
    return (HAL_I2C_Master_Transmit(&hi2c1, addr, (uint8_t*)b, n, I2C_TIMEOUT)==HAL_OK)
             ? PORT_OK : PORT_ERROR;
}

PortStatus_t Port_I2C_Receive(uint16_t addr,uint8_t *b,uint16_t n)
{
    return (HAL_I2C_Master_Receive(&hi2c1, addr, b, n, I2C_TIMEOUT)==HAL_OK)
             ? PORT_OK : PORT_ERROR;
}

/* ===== UART3 ================================================== */
static UART_HandleTypeDef huart3;
#define UART_TIMEOUT 100
#define UART_BAUDRATE 115200

PortStatus_t Port_UART_Init(void)
{
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef g = {
        .Pin       = GPIO_PIN_10 | GPIO_PIN_11,   /* PB10 = TX, PB11 = RX */
        .Mode      = GPIO_MODE_AF_PP,
        .Pull      = GPIO_NOPULL,
        .Speed     = GPIO_SPEED_FREQ_VERY_HIGH,
        .Alternate = GPIO_AF7_USART3
    };
    HAL_GPIO_Init(GPIOB, &g);

    huart3.Instance          = USART3;
    huart3.Init.BaudRate     = UART_BAUDRATE;
    huart3.Init.WordLength   = UART_WORDLENGTH_8B;
    huart3.Init.StopBits     = UART_STOPBITS_1;
    huart3.Init.Parity       = UART_PARITY_NONE;
    huart3.Init.Mode         = UART_MODE_TX_RX;

    return (HAL_UART_Init(&huart3)==HAL_OK) ? PORT_OK : PORT_ERROR;
}

PortStatus_t Port_UART_Send(const uint8_t *b,uint16_t n)
{
    return (HAL_UART_Transmit(&huart3,(uint8_t*)b,n,UART_TIMEOUT)==HAL_OK)
             ? PORT_OK : PORT_ERROR;
}

PortStatus_t Port_UART_Receive(uint8_t *b,uint16_t n)
{
    return (HAL_UART_Receive(&huart3,b,n,UART_TIMEOUT)==HAL_OK)
             ? PORT_OK : PORT_ERROR;
}

/* ===== ENCODER ================================================== */


PortStatus_t Port_Encoder_InitPins(GPIO_TypeDef *clkPort,uint16_t clkPin,
								   GPIO_TypeDef *dtPort	,uint16_t dtPin	,
								   GPIO_TypeDef *swPort	,uint16_t swPin )
{
	if (clkPort == GPIOC || dtPort == GPIOC || swPort == GPIOC)
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}

	GPIO_InitTypeDef g = {
			.Mode = GPIO_MODE_INPUT,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_LOW
	};

    g.Pin = clkPin; HAL_GPIO_Init(clkPort, &g);
    g.Pin = dtPin;  HAL_GPIO_Init(dtPort , &g);
    g.Pin = swPin;  HAL_GPIO_Init(swPort , &g);

    return PORT_OK;}


uint32_t Port_GetTick(void)
{
    return HAL_GetTick();
}

void Port_Delay(uint32_t delayTime)
{
    HAL_Delay(delayTime);
}

