#include "API_debounce.h"

static void Error_Handler(void);




static const uint32_t debounceTime = 40U;



void debounceFSM_init(stDebounce *button, gpio_t *pin, delay_t *delay)
{
	if (button == NULL || pin == NULL || delay == NULL)
	{
		Error_Handler();
	}

	button->gpio_ = pin;
	button->delay_ = delay;
	button->state_ = BUTTON_UP;
	button->isButtonPressed_ = false;

	delayInit(button->delay_, debounceTime);

	button->isInit = true;	
}

void debounceFSM_update(stDebounce *button)
{
	if (!button->isInit)
	{
		Error_Handler();
	}
	pin_state_t buttonState = Port_ReadPin(button->gpio_->port, button->gpio_->pin);

	switch (button->state_) {
	case BUTTON_UP:
		if (buttonState == GPIO_PIN_RESET)
		{
			delayWrite(button->delay_, debounceTime);
			button->state_ = BUTTON_FALLING;
		}
		break;
	case BUTTON_FALLING:
    if (delayRead(button->delay_))
    {
        if (buttonState == GPIO_PIN_RESET)
        {
            button->isButtonPressed_ = true;
            button->state_ = BUTTON_DOWN;
        }
        else
        {
            button->state_ = BUTTON_UP;
        }
    }
    break;
	case BUTTON_DOWN:
		if (buttonState == GPIO_PIN_SET)		
		{
			delayWrite(button->delay_, debounceTime);
			button->state_ = BUTTON_RAISING;
		}
		break;
	case BUTTON_RAISING:
		if (delayRead(button->delay_))
		{
			if (buttonState == GPIO_PIN_SET)
			{
				button->state_ = BUTTON_UP;
			}
			else
			{
				button->state_ = BUTTON_DOWN;
			}
		}
		break;
	default :
		debounceFSM_init(button, button->gpio_, button->delay_);
		break;
	}
}

bool_t debounceFSM_readKey(stDebounce *button)
{
    bool_t ret = button->isButtonPressed_;
    button->isButtonPressed_ = false;
    return ret; 
}

debounceState_t debounceFSM_getState(stDebounce *button)
{
	return button->state_;
}


static void Error_Handler(void)
{
	while (1)
	{
	}
}

    
