#include "API_encoder.h"
#include "API_uart.h"
#include "port.h"
#include "stm32f4xx_hal.h"

#define ENCODER_CLK_GPIO_Port   GPIOC
#define ENCODER_CLK_Pin         GPIO_PIN_10

#define ENCODER_DT_GPIO_Port    GPIOC
#define ENCODER_DT_Pin          GPIO_PIN_11

#define ENCODER_SW_GPIO_Port    GPIOC
#define ENCODER_SW_Pin          GPIO_PIN_12

typedef enum {
    ENCODER_STATE_00 = 0,
    ENCODER_STATE_01 = 2,
    ENCODER_STATE_11 = 3,
    ENCODER_STATE_10 = 1,
} Encoder_State_t;

typedef struct {
    gpio_t clk_, dt_, sw_;
    Encoder_State_t state_;
    Encoder_State_t prevState_;
    Encoder_Direction_t direction_;
    bool_t taken_;
} stEncoder;

static bool_t isInit_ = false;
static stEncoder  encoder_;
static stDebounce clkDebounce_;
static stDebounce dtDebounce_;
static stDebounce swDebounce_;
static delay_t clkDelay_;
static delay_t dtDelay_;
static delay_t swDelay_;
static gpio_t clkGpio_ = { .port = ENCODER_CLK_GPIO_Port, .pin = ENCODER_CLK_Pin };
static gpio_t dtGpio_  = { .port = ENCODER_DT_GPIO_Port,  .pin = ENCODER_DT_Pin  };
static gpio_t swGpio_  = { .port = ENCODER_SW_GPIO_Port,  .pin = ENCODER_SW_Pin  };
static bool_t buttonPressed = false;

Encoder_Status_t Encoder_Init(void)
{
    if (Port_Encoder_InitPins(ENCODER_CLK_GPIO_Port, ENCODER_CLK_Pin, ENCODER_DT_GPIO_Port, ENCODER_DT_Pin, ENCODER_SW_GPIO_Port, ENCODER_SW_Pin));

    encoder_.clk_ = clkGpio_;
    encoder_.dt_ = dtGpio_;
    encoder_.sw_ = swGpio_;

    debounceFSM_init(&clkDebounce_, &encoder_.clk_, &clkDelay_);
    debounceFSM_init(&dtDebounce_, &encoder_.dt_, &dtDelay_);
    debounceFSM_init(&swDebounce_, &encoder_.sw_, &swDelay_);

    encoder_.prevState_ = ENCODER_STATE_11;

    if (Port_ReadPin(encoder_.clk_.port, encoder_.clk_.pin) == GPIO_PIN_SET)
    {
        if (Port_ReadPin(encoder_.dt_.port, encoder_.dt_.pin) == GPIO_PIN_RESET)
        {
            encoder_.state_ = ENCODER_STATE_11;
        }
        else
        {
            encoder_.state_ = ENCODER_STATE_01;
        }        
    }
    else
    {
        if (Port_ReadPin(encoder_.dt_.port, encoder_.dt_.pin) == GPIO_PIN_SET)
        {
            encoder_.state_ = ENCODER_STATE_10;
        }
        else
        {
            encoder_.state_ = ENCODER_STATE_00;
            encoder_.prevState_ = ENCODER_STATE_00;
        }
    }
    
    encoder_.direction_ = ENCODER_CW;
    encoder_.taken_ = true;
    buttonPressed = false;
    isInit_ = true;
    return ENCODER_OK;
}

void Encoder_Update(void)
{
    if (!isInit_) return;

    debounceFSM_update(&clkDebounce_);
    debounceFSM_update(&dtDebounce_);
    debounceFSM_update(&swDebounce_);

    debounceState_t clkS = debounceFSM_getState(&clkDebounce_);
    debounceState_t dtS  = debounceFSM_getState(&dtDebounce_);

    switch (encoder_.state_)
    {
    case ENCODER_STATE_00:
        if(dtS == BUTTON_UP)
        {
            encoder_.state_ = ENCODER_STATE_10;
        }
        else if (clkS == BUTTON_UP)
        {
            encoder_.state_ = ENCODER_STATE_01;
        }        
        break;

    case ENCODER_STATE_01:
        if (dtS == BUTTON_UP)
        {
            encoder_.state_ = ENCODER_STATE_11;
            if (encoder_.prevState_ == ENCODER_STATE_00)
            {
                encoder_.prevState_ = ENCODER_STATE_11;
                encoder_.direction_ = ENCODER_CW;
                encoder_.taken_ = false;
            }            
        }
        else if (clkS == BUTTON_DOWN)
        {
            encoder_.state_ = ENCODER_STATE_00;
            if (encoder_.prevState_ == ENCODER_STATE_11)
            {
                encoder_.prevState_ = ENCODER_STATE_00;
                encoder_.direction_ = ENCODER_CCW;
                encoder_.taken_ = false;
            }            
        }
        break;

    case ENCODER_STATE_11:
        if (dtS == BUTTON_DOWN)
        {
            encoder_.state_ = ENCODER_STATE_01;
        }
        else if (clkS == BUTTON_DOWN)
        {
            encoder_.state_ = ENCODER_STATE_10;
        }
        break;
   
    case ENCODER_STATE_10:
        if (dtS == BUTTON_DOWN)
        {
            encoder_.state_ = ENCODER_STATE_00;
            if (encoder_.prevState_ == ENCODER_STATE_11)
            {
                encoder_.prevState_ = ENCODER_STATE_00;
                encoder_.direction_ = ENCODER_CW;
                encoder_.taken_ = false;
            }
        }
        else if (clkS == BUTTON_UP)
        {
            encoder_.state_ = ENCODER_STATE_11;
            if (encoder_.prevState_ == ENCODER_STATE_00)
            {
                encoder_.prevState_ = ENCODER_STATE_11;
                encoder_.direction_ = ENCODER_CCW;
                encoder_.taken_ = false;
            }
        }
        break;
        
    
    default:
        Encoder_Init();
        break;
    }

    if (debounceFSM_readKey(&swDebounce_))
        buttonPressed = true;

}

Encoder_Direction_t Encoder_GetDirection(void)
{
    Encoder_Direction_t d = ENCODER_DIR_NONE;

    if (!encoder_.taken_)
    {
        encoder_.taken_ = true;
        d = encoder_.direction_;
    }
    
    return d;
}

bool_t Encoder_IsPressed(void)
{
    bool_t p = buttonPressed;
    buttonPressed = false;
    return p;
}
