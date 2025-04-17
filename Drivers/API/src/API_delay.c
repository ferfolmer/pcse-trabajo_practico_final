#include "API_delay.h"
#include "stm32f4xx_hal.h"

void delayInit(delay_t * delay, tick_t duration)
{
	if (delay != NULL) {
		delay->duration = duration;
		delay->running = false;
		delay->startTime = HAL_GetTick();
	}
}

bool_t delayRead(delay_t * delay)
{
	bool ret = false;
	if (delay == NULL) {
		return ret;
	}

	if (delay->running == false)
	{
		delay->running = true;
		return ret;
	} else if ((HAL_GetTick() - delay->startTime) >= delay->duration)
		{
		delay->running = false;
		ret = true;
		}
	return ret;
}

void delayWrite(delay_t * delay, tick_t duration)
{
	if (delay != NULL) {
		delay->duration = duration;
		delay->startTime = HAL_GetTick();
	}
}

bool_t delayIsRunning(delay_t * delay)
{
	if (delay != NULL) {
		return delay->running;
	}
	return false; //prender led/sacar mensaje por uart/colgar el micro
}

