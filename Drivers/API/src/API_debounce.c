#include "API_debounce.h"
#include "stm32f4xx_hal.h"
#include "main.h"

typedef enum{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RAISING,
} debounceState_t;

static debounceState_t estado;
static delay_t delayAntirrebote;
static const uint32_t debounceTime = 40;
static bool_t deteccionFlancoDescendente = false;
static bool_t readButton(void);

/**@brief wrapper de la funcion HAL readPin para portar a nucleo 429 cambiar
 * B1_GPIO_Port -> USER_Btn_GPIO_Port
 * B1_Pin -> USER_Btn_Pin
 */
static bool_t readButton(void){
	return HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin);
}


void debounceFSM_init()
{
	estado = BUTTON_UP;
	delayInit(&delayAntirrebote, debounceTime); //definir const
}

void debounceFSM_update()
{
	switch (estado) {
	case BUTTON_UP:
		if (readButton())
		{
			delayWrite(&delayAntirrebote, debounceTime);
			estado = BUTTON_FALLING;
		}
		break;
	case BUTTON_FALLING:
    if (delayRead(&delayAntirrebote))
    {
        if (readButton())
        {
            deteccionFlancoDescendente = true;
            estado = BUTTON_DOWN;
        }
        else
        {
            estado = BUTTON_UP;
        }
    }
    break;
	case BUTTON_DOWN:
		if (!readButton())
		{
			delayWrite(&delayAntirrebote, debounceTime);
			estado = BUTTON_RAISING;
		}
		break;
	case BUTTON_RAISING:
		if (delayRead(&delayAntirrebote))
		{
			if (!readButton())
			{
				estado = BUTTON_UP;
			}
			else
			{
				estado = BUTTON_DOWN;
			}
		}
		break;
	default :
		debounceFSM_init();
		break;
	}
}

bool_t readKey()
{
    bool_t ret = deteccionFlancoDescendente;
    deteccionFlancoDescendente = false;
    return ret; 
}


    
