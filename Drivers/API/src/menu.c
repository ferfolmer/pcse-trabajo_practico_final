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
            LCD_Print("BME280 Init OK");
            uartSendString((uint8_t *)"BME280 Init OK\r\n");
            menu_.state_ = STATE_DISPLAY_DATA;
            stateEntry = true;
        }
        break;

    case STATE_DISPLAY_DATA:
        if (stateEntry)
        {
            BME280_ReadCorrected(&sensor_);
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            updateDisplayData(&sensor_.data);
            delayWrite(&dataDelay_, MENU_DATA_UPDATE_TIME);
            stateEntry = false;
        }
        
        if (delayRead(&dataDelay_))
        {
            BME280_ReadCorrected(&sensor_);
            updateDisplayData(&sensor_.data);
            uartSendString((uint8_t *)"Polling sensor\r\n");
            delayWrite(&dataDelay_, MENU_DATA_UPDATE_TIME);
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
            LCD_Print(" Calibracion >");
            uartSendString((uint8_t *)"ENTRO EN CALIBRACION\r\n");
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
            LCD_Print("  Reset calibr ");
            uartSendString((uint8_t *)"ENTRO EN RESET CALIB \r\n");
            stateEntry = false;
        }

        if (btnPressed)
        {
            menu_.state_ = STATE_DISPLAY_DATA;
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
            LCD_Print(" Calib Tempera>");
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
            LCD_Print("<Calib Presion>");
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
            LCD_Print("<Calib Humedad>");
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
            LCD_Print("Offset Temperat");
            offset_ = sensor_.offsets.temperatureOff_x100;
            updateOffsetSelect(offset_, menu_.state_);
            uartSendString((uint8_t *)"ENTRO EN OFFSET TEMP\r\n");
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
            menu_.state_ = STATE_DISPLAY_DATA;
            sensor_.offsets.temperatureOff_x100 = offset_;
            uartSendString((uint8_t *)"OFFSET TEMP ACTUALIZADO");
            stateEntry = true;
        }
        break;

    case STATE_OFFSET_PRESS:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Offset Presion ");
            offset_ = sensor_.offsets.pressureOff;
            updateOffsetSelect(offset_, menu_.state_);
            uartSendString((uint8_t *)"ENTRO EN OFFSET PRES\r\n");
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
            menu_.state_ = STATE_DISPLAY_DATA;
            sensor_.offsets.pressureOff = offset_;
            stateEntry = true;
        }
        break;

    case STATE_OFFSET_HUM:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("Offset Humedad ");
            uartSendString((uint8_t *)"ENTRO EN OFFSET HUM\r\n");
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
            menu_.state_ = STATE_DISPLAY_DATA;
            sensor_.offsets.humidityOff_x1024 = offset_;
            stateEntry = true;
        }
        break;

    case STATE_EXIT:
        if (stateEntry)
        {
            LCD_Clear();
            LCD_SetCursor(LINE_1, 0);
            LCD_Print("< Salir");
            uartSendString((uint8_t *)"ENTRO EN SALIR\r\n");
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

void BME280_ReadCorrected(BME280_t *b)
{
    if (BME280_ReadData(&b->data) != BME280_OK) return;
    b->data.temperature_x100 += b->offsets.temperatureOff_x100;
    b->data.pressure         += b->offsets.pressureOff;
    b->data.humidity_x1024   += b->offsets.humidityOff_x1024;
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


