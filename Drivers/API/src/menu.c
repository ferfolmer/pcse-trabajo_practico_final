#include "menu.h"
#include "API_delay.h"
#include "API_lcd.h"
#include "API_bme280.h"
#include "API_uart.h"

#define MENU_LCD_STR_INIT   "BME280 Init OK"
#define MENU_LCD_STR_DATA   "Datos BME280"
#define MENU_LCD_STR_CALIB  "Calibracion "
#define MENU_LCD_STR_RESET  "Reset calibr "
#define MENU_LCD_STR_TEMP   " Calib Tempera>"
#define MENU_LCD_STR_PRESS  "<Calib Presion>"
#define MENU_LCD_STR_HUM    "<Calib Humedad>"
#define MENU_LCD_STR_EXIT   "< Salir"
#define MENU_LCD_STR_OFFSET_TEMP   "Offset Temp   "
#define MENU_LCD_STR_OFFSET_PRESS  "Offset Presion "
#define MENU_LCD_STR_OFFSET_HUM    "Offset Humedad "

#define MENU_UART_STR_INIT                  (uint8_t *)"BME280 Init OK\r\n"
#define MENU_UART_STR_DATA_TOP              (uint8_t *)"Polling sensor\r\n"
#define MENU_UART_STR_DATA                  (uint8_t *)"ENTRO EN MOSTRAR DATOS\r\n"
#define MENU_UART_STR_CALIB                 (uint8_t *)"ENTRO EN CALIBRACION\r\n"
#define MENU_UART_STR_RESET                 (uint8_t *)"ENTRO EN RESET CALIB \r\n"
#define MENU_UART_STR_TEMP                  (uint8_t *)"ENTRO EN CALIBRACION TEMP\r\n"
#define MENU_UART_STR_PRESS                 (uint8_t *)"ENTRO EN CALIBRACION PRES\r\n"
#define MENU_UART_STR_HUM                   (uint8_t *)"ENTRO EN CALIBRACION HUM\r\n"
#define MENU_UART_STR_OFFSET_TEMP           (uint8_t *)"ENTRO EN OFFSET TEMP\r\n"
#define MENU_UART_STR_OFFSET_PRESS          (uint8_t *)"ENTRO EN OFFSET PRES\r\n"
#define MENU_UART_STR_OFFSET_HUM            (uint8_t *)"ENTRO EN OFFSET HUM\r\n"
#define MENU_UART_STR_EXIT                  (uint8_t *)"ENTRO EN SALIR\r\n"
#define MENU_UART_STR_SET_TEMP_OFFSET_OK    (uint8_t *)"OFFSET TEMP ACTUALIZADO\r\n"
#define MENU_UART_STR_SET_PRESS_OFFSET_OK   (uint8_t *)"OFFSET PRES ACTUALIZADO\r\n"
#define MENU_UART_STR_SET_HUM_OFFSET_OK     (uint8_t *)"OFFSET HUM ACTUALIZADO\r\n"

#define LINE_1 0
#define LINE_2 1
#define TEMP_POS 0
#define PRESS_POS 3
#define HUM_POS 9
#define OFFSET_POS 8
#define LCD_BUFFER_SIZE 17

#define TEMP_MIN_OFFSET 10  //0.1ºC
#define HUM_MIN_OFFSET  1024 //1%HR
#define PRES_MIN_OFFSET 100   //Pa
delay_t dataDelay_;

static stMenu menu_;
static BME280_t sensor_ = { .offsets = {0,0,0}};
static int32_t offset_;


static void Error_Handler()
{
    while (1)
    {
    }
}

static void updateDisplayData(BME280_Data_t *data);
static void updateOffsetSelect(int16_t offset, MenuState_t state);

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
        {
            LCD_SetCursor(0, 0);
            LCD_Print(MENU_LCD_STR_INIT);
            uartSendString(MENU_UART_STR_INIT);
            menu_.state_ = STATE_DISPLAY_DATA_TOP;
            stateEntry = true;
        }
        break;

    case STATE_DISPLAY_DATA_TOP:
        if (stateEntry)
        {
            BME280_ReadCorrected(&sensor_);
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            updateDisplayData(&sensor_.data);
            // delayWrite(&dataDelay_, MENU_DATA_UPDATE_TIME);
            stateEntry = false;
        }
        
        if (delayRead(&dataDelay_))
        {
            BME280_ReadCorrected(&sensor_);
            updateDisplayData(&sensor_.data);
            uartSendString(MENU_UART_STR_DATA_TOP);
            delayWrite(&dataDelay_, MENU_DATA_UPDATE_TIME);
        }

        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
        break;

    case STATE_DISPLAY_DATA:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_DATA);
            uartSendString(MENU_UART_STR_DATA);
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA_TOP;
            stateEntry = true;
        }

        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_CALIBRATION;
            stateEntry = true;
        }
        
        if (dir == ENCODER_CCW)
        {
            menu_.state_ = STATE_RESET_CALIBRATION;
            stateEntry = true;
        }
        break;

    case STATE_CALIBRATION:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_CALIB);
            uartSendString(MENU_UART_STR_CALIB);
            stateEntry = false;
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_CALIBRATION_TEMP;
            stateEntry = true;
        }
        
        if (dir == ENCODER_CCW)
        {
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
        
        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_RESET_CALIBRATION;
            stateEntry = true;
        }
        break;

    case STATE_RESET_CALIBRATION:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_RESET);
            uartSendString(MENU_UART_STR_RESET);
            stateEntry = false;
        }

        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA_TOP;
            sensor_.offsets.humidityOff_x1024 = 0U;
            sensor_.offsets.pressureOff = 0U;
            sensor_.offsets.temperatureOff_x100 = 0U;
            stateEntry = true;
        }

        if (dir == ENCODER_CW)
        {
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
       
        if (dir == ENCODER_CCW)
        {
            menu_.state_ = STATE_CALIBRATION;
            stateEntry = true;
        }

        
        break;
        
    case STATE_CALIBRATION_TEMP:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_TEMP);
            uartSendString(MENU_UART_STR_TEMP);
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
            LCD_Print(MENU_LCD_STR_PRESS);
            uartSendString(MENU_UART_STR_PRESS);
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
            LCD_Print(MENU_LCD_STR_HUM);
            uartSendString(MENU_UART_STR_HUM);
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
            LCD_Print(MENU_LCD_STR_OFFSET_TEMP);
            offset_ = sensor_.offsets.temperatureOff_x100;
            updateOffsetSelect(offset_, menu_.state_);
            uartSendString(MENU_UART_STR_OFFSET_TEMP);
            stateEntry = false;
        }
        
        if (dir != ENCODER_DIR_NONE)
        {
        	if (dir == ENCODER_CW)
        	{
        		offset_+= TEMP_MIN_OFFSET;
        	}
        	else
        	{
        		offset_-= TEMP_MIN_OFFSET;
        	}
        	updateOffsetSelect(offset_, menu_.state_);
        }

        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA_TOP;
            sensor_.offsets.temperatureOff_x100 = offset_;
            uartSendString(MENU_UART_STR_SET_TEMP_OFFSET_OK);
            stateEntry = true;
        }
        break;

    case STATE_OFFSET_PRESS:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_OFFSET_PRESS);
            offset_ = sensor_.offsets.pressureOff;
            updateOffsetSelect(offset_, menu_.state_);
            uartSendString(MENU_UART_STR_OFFSET_PRESS);
            stateEntry = false;
        }        
        
        if (dir != ENCODER_DIR_NONE)
        {
        	if (dir == ENCODER_CW)
        	{
        		offset_+= PRES_MIN_OFFSET;
        	}
        	else
        	{
        		offset_-= PRES_MIN_OFFSET;
        	}
        	updateOffsetSelect(offset_, menu_.state_);
        }

        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA_TOP;
            uartSendString(MENU_UART_STR_SET_PRESS_OFFSET_OK);
            sensor_.offsets.pressureOff = offset_;
            stateEntry = true;
        }
        break;

    case STATE_OFFSET_HUM:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_OFFSET_HUM);
            uartSendString(MENU_UART_STR_OFFSET_HUM);
            offset_ = sensor_.offsets.humidityOff_x1024;
            updateOffsetSelect(offset_, menu_.state_);
            stateEntry = false;
        }

        if (dir != ENCODER_DIR_NONE)
        {
        	if (dir == ENCODER_CW)
        	{
        		offset_+= HUM_MIN_OFFSET;
        	}
        	else
        	{
        		offset_-= HUM_MIN_OFFSET;
        	}
        	updateOffsetSelect(offset_, menu_.state_);
        }
        
        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA_TOP;
            uartSendString(MENU_UART_STR_SET_HUM_OFFSET_OK);
            sensor_.offsets.humidityOff_x1024 = offset_;
            stateEntry = true;
        }
        break;

    case STATE_EXIT:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print(MENU_LCD_STR_EXIT);
            uartSendString(MENU_UART_STR_EXIT);
            stateEntry = false;
        }

        if (dir == ENCODER_CCW)
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
    snprintf(buffer, sizeof(buffer),"T:%" PRId32 ".%1" PRId32 "\xDF""C", t_int, t_dec);
    LCD_SetCursor(LINE_1, TEMP_POS);    
    LCD_Print(buffer);

    uint32_t h_int = data->humidity_x1024 / 1024;
//    uint32_t h_dec = (data->humidity_x1024 % 1024) * 10 / 1024;
    snprintf(buffer, sizeof(buffer),"H:%" PRIu32 "%%", h_int);//snprintf(buffer, sizeof(buffer),"H:%lu.%1lu%%",(unsigned long)h_int,(unsigned long)h_dec);
    LCD_SetCursor(LINE_1, HUM_POS);
    LCD_Print(buffer);

    uint32_t p_int = data->pressure / 100;
    uint32_t p_dec = (data->pressure % 100) / 10;
    snprintf(buffer, sizeof(buffer),"P:%" PRIu32 ".%1" PRIu32 "hPa", p_int, p_dec);
    LCD_SetCursor(LINE_2, PRESS_POS);
    LCD_Print(buffer);
}

static void updateOffsetSelect(int16_t offset, MenuState_t state)
{
    char buffer[LCD_BUFFER_SIZE];
    char sign = (offset < 0) ? '-' : '+';
    int16_t absOff = (offset < 0) ? -offset : offset;
    LCD_SetCursor(LINE_2, OFFSET_POS);

    switch (state)
    {
    case STATE_OFFSET_TEMP:
        {
            int16_t integer = (absOff / 100);
            int16_t decimal = (absOff % 100) / 10;
            sprintf(buffer, "%c%d.%1d \xDF""C", sign, integer, decimal);
            LCD_SetCursor(LINE_2, OFFSET_POS);
            LCD_Print(buffer);
        }
        break;

    case STATE_OFFSET_PRESS:
        sprintf(buffer, "%c%d hPa", sign, (absOff / 100));
        LCD_SetCursor(LINE_2, OFFSET_POS);
        LCD_Print(buffer);
    break;
    
    case STATE_OFFSET_HUM:
        sprintf(buffer, "%c%d%%", sign, (absOff / 1024));
        LCD_SetCursor(LINE_2, OFFSET_POS);
        LCD_Print(buffer);
        break;
    
    default:
        Error_Handler();
        break;
    }
}


