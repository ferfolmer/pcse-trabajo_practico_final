#include "API_uart.h"
#include "port.h"


#define UART_INSTANCE USART3
#define UART_BAUDRATE 115200
#define UART_TIMEOUT 100

static bool_t isInit_ = false;
static const char *uartInitMessage = "UART Initialized with config 8N1\r\n";


/**
 * @brief inicializa la instancia utilizada para la comunicacion serie
 * 
 * @return bool_t true si la inicializacion fue exitosa, false en caso contrario
 */
bool_t uartInit() {
    bool_t ret = false;

    if (isInit_)
    {
    	return true;
    }

    if (Port_UART_Init() != PORT_OK)
	{
		return ret;
	}
    isInit_ = true;
    Port_UART_Send((const uint8_t *)uartInitMessage, strlen(uartInitMessage));

    return ret;
}

/**
 * @brief Envia una cadena de caracteres por UART
 * 
 * @param pstring puntero a la cadena de caracteres a enviar
 */
void uartSendString(uint8_t * pstring) {
	if (isInit_) Port_UART_Send(pstring, strlen((char *)pstring));
}

/**
 * @brief Envia una cadena de caracteres de tamaño fijo por UART
 * 
 * @param pstring puntero a la cadena de caracteres a enviar
 * @param size tamaño de la cadena a enviar
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size) {
	if (isInit_) Port_UART_Send(pstring, size);
}

/**
 * @brief Recibe una cadena de caracteres de tamaño fijo por UART
 * 
 * @param pstring puntero a la cadena de caracteres a recibir
 * @param size tamaño de la cadena a recibir
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size) {
	if (isInit_) Port_UART_Receive(pstring, size);
}
