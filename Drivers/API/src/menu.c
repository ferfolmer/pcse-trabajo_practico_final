#include "menu.h"
#include "API_delay.h"
#include "API_lcd.h"
#include "API_bme280.h"
#include "API_uart.h"


#define LINE_1 0
#define LINE_2 1
#define TEMP_POS 0
#define PRESS_POS 3
#define HUM_POS 9
#define LCD_BUFFER_SIZE 17
delay_t dataDelay_;

static stMenu menu_;
static BME280_Data_t bme280Data_;

static void Error_Handler()
{
    while (1)
    {
    }
}

static void updateDisplayData(BME280_Data_t *data);

void Menu_Init(void)
{
    menu_.state_ = STATE_INIT;
    delayInit(&dataDelay_, MENU_DATA_UPDATE_TIME);

    if (LCD_Init() != LCD_OK)
    {
        Error_Handler();
    }

    LCD_Clear();

    if (BME280_Init() != BME280_OK)
    {
        Error_Handler();
    }

    Encoder_Init();
    uartInit();
}

static bool_t stateEntry = false;

void Menu_Update(void)
{

    Encoder_Update();
    Encoder_Direction_t dir = Encoder_GetDirection();
    bool_t btnPressed = Encoder_IsPressed();

    switch (menu_.state_)
    {
    case STATE_INIT:
        if (BME280_ReadData(&bme280Data_) == BME280_OK)
        {
            LCD_SetCursor(0, 0);
            LCD_Print("BME280 Init OK");
            uartSendString((uint8_t *)"BME280 Init OK\r\n");
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
        break;

    case STATE_DISPLAY_DATA:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            updateDisplayData(&bme280Data_);
            delayWrite(&dataDelay_, MENU_DATA_UPDATE_TIME);
            stateEntry = false;
        }
        
        if (delayRead(&dataDelay_))
        {
            BME280_ReadData(&bme280Data_);
            updateDisplayData(&bme280Data_);
            uartSendString((uint8_t *)"Polling sensor\r\n");
            delayWrite(&dataDelay_, MENU_DATA_UPDATE_TIME);
        }

        if (dir == ENCODER_CW || dir == ENCODER_CCW)       
        {
            menu_.state_ = STATE_CALIBRATION;
            stateEntry = true;
        }
        break;

    case STATE_CALIBRATION:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Calibracion >");
            uartSendString((uint8_t *)"ENTRO EN CALIBRACION\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_CALIBRATION_TEMP;
            stateEntry = true;
        }
        
        if (dir == ENCODER_CW || dir == ENCODER_CCW)
        {
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
        break;
        
    case STATE_CALIBRATION_TEMP:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Calibracion Temp >");
            uartSendString((uint8_t *)"ENTRO EN CALIBRACION TEMP\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_OFFSET_TEMP;
            stateEntry = true;
        }

        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_CALIBRATION_PRESS;
            stateEntry = true;
        }     
        break;

    case STATE_CALIBRATION_PRESS:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Calibracion Pres >");
            uartSendString((uint8_t *)"ENTRO EN CALIBRACION PRES\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_OFFSET_PRESS;
            stateEntry = true;
        }

        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_CALIBRATION_HUM;
            stateEntry = true;
        }
        else if (dir == ENCODER_CCW)
        {
            menu_.state_ = STATE_CALIBRATION_TEMP;
            stateEntry = true;
        }
        break;

    case STATE_CALIBRATION_HUM:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Calibracion Hum >");
            uartSendString((uint8_t *)"ENTRO EN CALIBRACION HUM\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_OFFSET_HUM;
            stateEntry = true;
        }

        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_EXIT;
            stateEntry = true;
        }
        else if (dir == ENCODER_CCW)
        {
            menu_.state_ = STATE_CALIBRATION_PRESS;
            stateEntry = true;
        }              
        break;

    case STATE_OFFSET_TEMP:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Offset Temp >");
            uartSendString((uint8_t *)"ENTRO EN OFFSET TEMP\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_CALIBRATION_TEMP;
            stateEntry = true;
        }
        break;

    case STATE_OFFSET_PRESS:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Offset Pres >");
            uartSendString((uint8_t *)"ENTRO EN OFFSET PRES\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_CALIBRATION_PRESS;
            stateEntry = true;
        }
        break;

    case STATE_OFFSET_HUM:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Offset Hum >");
            uartSendString((uint8_t *)"ENTRO EN OFFSET HUM\r\n");
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_CALIBRATION_HUM;
            stateEntry = true;
        }
        break;

    case STATE_EXIT:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Salir");
            uartSendString((uint8_t *)"ENTRO EN SALIR\r\n");
            stateEntry = false;
        }

        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_CALIBRATION_HUM;
            stateEntry = true;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
        break;
    
    default:
        break;
    }
}

static void updateDisplayData(BME280_Data_t *data)
{
    char buffer[LCD_BUFFER_SIZE];

    int32_t t_int = data->temperature_x100 / 100;
    int32_t t_dec = abs(data->temperature_x100 % 100) / 10;
    snprintf(buffer, sizeof(buffer),"T:%ld.%1ld\xDF""C",(long)t_int, (long)t_dec);
    LCD_SetCursor(LINE_1, TEMP_POS);    
    LCD_Print(buffer);

    uint32_t h_int = data->humidity_x1024 % 1024;
    uint32_t h_dec = (data->humidity_x1024 * 10 + 512) / 1024;
    snprintf(buffer, sizeof(buffer),"H:%lu.%1lu%%",(unsigned long)h_int,(unsigned long)h_dec);
    LCD_SetCursor(LINE_1, HUM_POS);
    LCD_Print(buffer);

    uint32_t p_int = data->pressure / 100;
    uint32_t p_dec = (data->pressure % 100) / 10;
    snprintf(buffer, sizeof(buffer),"P:%lu.%1luhPa",(unsigned long)p_int,(unsigned long)p_dec);
    LCD_SetCursor(LINE_2, PRESS_POS);
    LCD_Print(buffer);
}


