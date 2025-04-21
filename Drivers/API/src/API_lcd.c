#include "API_lcd.h"
#include "API_i2c.h"

//-----------------------------------------------------------------------------
// Definiciones internas para manejo de bits de control
//-----------------------------------------------------------------------------
#define LCD_PIN_RS               (1U << 0)     /* P0 */
#define LCD_PIN_RW               (1U << 1)     /* P1 */
#define LCD_PIN_EN               (1U << 2)     /* P2 */
#define LCD_PIN_BACKLIGHT        (1U << 3)     /* P3 */

#define LCD_I2C_ADDRESS          (0x27 << 1)

#define LCD_INIT_CMD_1           (0x03)
#define LCD_INIT_CMD_2           (0x02)


// En algunos módulos, los 4 bits de datos se ubican en la parte alta del byte,
// pero puede variar. Ajusta si no funciona con tu hardware.

//-----------------------------------------------------------------------------
// Variables estáticas
//-----------------------------------------------------------------------------
static const uint8_t LCD_INIT_CMD[] = {
	MODE_4BIT,
	DISPLAY_CONTROL,
    CLR_LCD,
    ENTRY_MODE | AUTOINCREMENT,
    DISPLAY_CONTROL | DISPLAY_ON,
};
static uint8_t lcd_bl_mask = LCD_PIN_BACKLIGHT;
//-----------------------------------------------------------------------------
// Prototipos de funciones internas
//-----------------------------------------------------------------------------
static void LCD_Send4Bits(uint8_t nibble, uint8_t control);
static void LCD_SendCmd(uint8_t cmd);
static void LCD_SendData(uint8_t data);
static void LCD_WriteI2C(uint8_t dato, uint16_t len);
static void LCD_Delay(uint32_t ms);
static inline void LCD_DelayUs(uint16_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1e6) * us;
    while (DWT->CYCCNT - start < ticks) { __NOP(); }
}


LCD_Status_t LCD_Init(void)
{
    LCD_Status_t status = LCD_ERROR;
    if (I2C_isInit() != I2C_OK)
    {
        if (I2C_Init() != I2C_OK)
        {
            return status;  // Error: I2C no inicializado
        }
    }

    LCD_Delay(DELAY_20MS);

    // -------------------------------------------------------
    // Secuencia de "despertar" en 4 bits:
    // - Enviar 0x03 (nibble alto 0x3) tres veces con retardos
    // - Finalmente, enviar 0x02 (para 4-bit mode)
    // -------------------------------------------------------
    LCD_Send4Bits(LCD_INIT_CMD_1, LCD_CONTROL);  
    LCD_Delay(DELAY_10MS);
    LCD_Send4Bits(LCD_INIT_CMD_1, LCD_CONTROL);
    LCD_Delay(DELAY_1MS);
    LCD_Send4Bits(LCD_INIT_CMD_1, LCD_CONTROL);
    LCD_Send4Bits(LCD_INIT_CMD_2, LCD_CONTROL);  // Activar modo 4 bits

    for (uint8_t i = 0; i < sizeof(LCD_INIT_CMD); i++) {
        LCD_SendCmd(LCD_INIT_CMD[i]);
    
        if (LCD_INIT_CMD[i] == CLR_LCD || LCD_INIT_CMD[i] == RETURN_HOME)
            LCD_Delay(DELAY_2MS);
    }

    status = LCD_OK;
    return status;
}

LCD_Status_t LCD_Clear(void)
{
    LCD_Status_t status = LCD_ERROR;
    LCD_SendCmd(CLR_LCD);   // Clear display
    LCD_Delay(DELAY_2MS);
    status = LCD_OK;
    return status;
}

LCD_Status_t LCD_SetCursor(uint8_t row, uint8_t col)
{
    LCD_Status_t status = LCD_ERROR;
    if (row > 1 || col > 15)
    {
        return status;  // Error: fila o columna fuera de rango
    }
    // Dirección base para cada línea (según HD44780)
    uint8_t baseAddress[2] = {LCD_LINE_1, LCD_LINE_2};
    if (row > 1) row = 1;
    LCD_SendCmd(baseAddress[row] + col);
    status = LCD_OK;
    
    return status;
}

void LCD_Print(const char *str)
{
    while (*str)
    {
        LCD_SendData((uint8_t)*str);
        str++;
    }
}

//-----------------------------------------------------------------------------
// Implementación de funciones internas
//-----------------------------------------------------------------------------

/**
 * @brief Envía medio byte (nibble) al LCD, junto con bits de control
 *        (RS, RW, EN). Se asume siempre RW=0 (escritura).
 * @param nibble valor de 4 bits (0x0..0xF)
 * @param control si RS=1 es dato, si RS=0 es comando
 */
static void LCD_Send4Bits(uint8_t value, uint8_t control)
{
    // nibble & 0x0F para asegurar solo 4 bits
    // control: RS=1 / 0, RW=0, EN se activa en el pulso
    uint8_t byte = ((value & LCD_DATA_MASK_LOW) << LCD_DATA_SHIFT);
    // Combina con RS
    if (control) byte |= LCD_PIN_RS;

    byte |= lcd_bl_mask;  // Agrega el bit de retroiluminación
    LCD_WriteI2C(byte | LCD_PIN_EN, sizeof(byte));
    LCD_Delay(DELAY_1MS);
	LCD_WriteI2C(byte & ~LCD_PIN_EN, sizeof(byte));
    LCD_Delay(DELAY_1MS);
}

static void LCD_Send8Bits(uint8_t value, uint8_t control)
{
    LCD_Send4Bits((value & LCD_DATA_MASK_HIGH) >> LCD_DATA_SHIFT, control);
    LCD_Send4Bits(value & LCD_DATA_MASK_LOW, control);
}

/**
 * @brief Enviar un comando (RS=0).
 */
static void LCD_SendCmd(uint8_t cmd)
{
    LCD_Send8Bits(cmd, LCD_CONTROL);
}

/**
 * @brief Enviar un dato (RS=1).
 */
static void LCD_SendData(uint8_t data)
{
    LCD_Send8Bits(data, LCD_DATA);
}

/**
 * @brief Escribe un byte a través de I2C hacia el PCF8574.
 */
static void LCD_WriteI2C(uint8_t dato, uint16_t len)
{
    // Transmite un byte al dispositivo en la dirección LCD_I2C_ADDRESS
    I2C_Send(LCD_I2C_ADDRESS, &dato, len);
    // Se asume que no hay error de timeout. Podrías evaluar el retorno si deseas.
}

/**
 * @brief Delay sencillo que reusa HAL_Delay (milisegundos).
 */
static void LCD_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}
