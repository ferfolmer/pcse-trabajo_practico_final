#include "API_uart.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#define UART_INSTANCE USART3
#define UART_BAUDRATE 115200
#define UART_TIMEOUT 100

static UART_HandleTypeDef huart_;
static const char *uartInitMessage = "UART Initialized with config 8N1\r\n";


/**
 * @brief inicializa la instancia utilizada para la comunicacion serie
 * 
 * @return bool_t true si la inicializacion fue exitosa, false en caso contrario
 */
bool_t uartInit() {
    bool_t ret = false;
    huart_.Instance = UART_INSTANCE;
    huart_.Init.BaudRate = UART_BAUDRATE;
    huart_.Init.WordLength = UART_WORDLENGTH_8B;
    huart_.Init.StopBits = UART_STOPBITS_1;
    huart_.Init.Parity = UART_PARITY_NONE;
    huart_.Init.Mode = UART_MODE_TX_RX;
    huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart_) != HAL_OK) {
        ret = false; // Initialization failed
    } else {
        uartSendString((uint8_t *)uartInitMessage);
        ret = true;
        // UART initialized successfully
    }
    return ret;
}

/**
 * @brief Envia una cadena de caracteres por UART
 * 
 * @param pstring puntero a la cadena de caracteres a enviar
 */
void uartSendString(uint8_t * pstring) {
    HAL_UART_Transmit(&huart_, pstring, strlen((char *)pstring), UART_TIMEOUT);
}

/**
 * @brief Envia una cadena de caracteres de tamaño fijo por UART
 * 
 * @param pstring puntero a la cadena de caracteres a enviar
 * @param size tamaño de la cadena a enviar
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size) {
    HAL_UART_Transmit(&huart_, pstring, size, UART_TIMEOUT);
}

/**
 * @brief Recibe una cadena de caracteres de tamaño fijo por UART
 * 
 * @param pstring puntero a la cadena de caracteres a recibir
 * @param size tamaño de la cadena a recibir
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size) {
    HAL_UART_Receive(&huart_, pstring, size, UART_TIMEOUT);
}