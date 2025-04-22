#include "port.h"
#include "API_i2c.h"

static bool_t isInit_ = false;

//static void Error_Handler(void);


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
    isInit_ = (Port_I2C_Init() == PORT_OK);
    return isInit_ ? I2C_OK : I2C_ERROR;
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
	return (Port_I2C_Send(address, data, size) == PORT_OK) ? I2C_OK : I2C_ERROR;
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
	return (Port_I2C_Receive(address, data, size) == PORT_OK) ? I2C_OK : I2C_ERROR;
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



/**
 * @brief Error handler function that is called when an invalid operation occurs
 * 
 */
//static void Error_Handler(void)
//{
//    while(1)
//    {}
//}
