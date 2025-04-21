#include "API_encoder.h"
#include "API_uart.h"
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
    Encoder_Direction_t direction_;
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
static bool_t prevClkState = false;

static void Encoder_GPIO_Init(void);

Encoder_Status_t Encoder_Init(void)
{
    Encoder_GPIO_Init();

    encoder_.clk_ = clkGpio_;
    encoder_.dt_ = dtGpio_;
    encoder_.sw_ = swGpio_;

    debounceFSM_init(&clkDebounce_, &encoder_.clk_, &clkDelay_);
    debounceFSM_init(&dtDebounce_, &encoder_.dt_, &dtDelay_);
    debounceFSM_init(&swDebounce_, &encoder_.sw_, &swDelay_);

    debounceState_t s = debounceFSM_getState(&clkDebounce_);
    prevClkState = (s == BUTTON_UP);



    encoder_.direction_ = ENCODER_DIR_NONE;
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

    bool clkStableUp   = (clkS == BUTTON_UP);
    bool clkStableDown = (clkS == BUTTON_DOWN);
    bool dtStableUp    = (dtS  == BUTTON_UP);

    /* flanco 0→1 sólo si ambos prev y new fueron ESTABLES */
    if (prevClkState == false && clkStableUp)
    {
        encoder_.direction_ = dtStableUp ? ENCODER_CCW : ENCODER_CW;
    }

    /* actualizamos prev sólo si está en un estado estable */
    if (clkStableUp || clkStableDown)
    {
        prevClkState = clkStableUp;
    }

    if (debounceFSM_readKey(&swDebounce_))
        buttonPressed = true;

}

Encoder_Direction_t Encoder_GetDirection(void)
{
    Encoder_Direction_t d = encoder_.direction_;
    if (d == ENCODER_CW)
        uartSendString((uint8_t *)"Encoder dir: CW\r\n");
    else if (d == ENCODER_CCW)
        uartSendString((uint8_t *)"Encoder dir: CCW\r\n");
    else
    encoder_.direction_ = ENCODER_DIR_NONE;
    return d;
}

bool_t Encoder_IsPressed(void)
{
    bool_t p = buttonPressed;
    buttonPressed = false;
    return p;
}

static void Encoder_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin   = ENCODER_CLK_Pin | ENCODER_DT_Pin | ENCODER_SW_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
