#ifndef API_LCD_H
#define API_LCD_H

#include "stm32f4xx_hal.h"

typedef enum {
    LCD_OK = 0,
    LCD_ERROR,         
} LCD_Status_t;

#define DATA    1
#define CONTROL 0

#define LCD_I2C_ADDRESS   (0x27 << 1)

#define LCD_DATA_SHIFT          4U
#define LCD_DATA_MASK_HIGH     (0xF0) // Máscara para el nibble alto
#define LCD_DATA_MASK_LOW      (0x0F) // Máscara para el nibble bajo

#define LCD_LINE_1      (0x80) // Dirección de la primera línea
#define LCD_LINE_2      (0xC0) // Dirección de la segunda línea

#define CLR_LCD              (1 << 0)
#define RETURN_HOME          (1 << 1)
#define ENTRY_MODE           (1 << 2)
#define DISPLAY_CONTROL      (1 << 3)
#define CURSOR_DISPLAY_SHIFT (1 << 4)
#define FUNTION_SET          (1 << 5)
#define SET_CGRAM            (1 << 6)
#define SET_DDRAM            (1 << 7)

#define CURSOR_BLINK         (1 << 0)
#define CURSOR_ON            (1 << 1)
#define DISPLAY_ON           (1 << 2)
#define AUTOINCREMENT        (1 << 1)
#define MODE_4BIT             0x28

//Delays
#define DELAY_20MS  20
#define DELAY_10MS  10
#define DELAY_2MS   2
#define DELAY_1MS   1
#define DELAY_50US 	50



/**
 * @brief  Inicializa el LCD en modo 4 bits a través de PCF8574 (I2C).
 * @param  hi2c Puntero al handle de la I2C configurada.
 */
LCD_Status_t LCD_Init(I2C_HandleTypeDef *hi2c);

LCD_Status_t LCD_SendChar(uint8_t data);

LCD_Status_t LCD_SendString(char *str);

LCD_Status_t LCD_SendInt(int32_t num);

/**
 * @brief  Limpia la pantalla y ubica el cursor en la posición (0,0).
 */
LCD_Status_t LCD_Clear(void);

/**
 * @brief  Ubica el cursor en la fila y columna especificadas.
 *         - row puede ser 0 o 1 (para un LCD 16x2).
 *         - col va de 0 a 15.
 */
LCD_Status_t LCD_SetCursor(uint8_t row, uint8_t col);

/**
 * @brief  Imprime una cadena de texto en la posición actual del cursor.
 */
void LCD_Print(const char *str);

#endif // API_LCD_H
