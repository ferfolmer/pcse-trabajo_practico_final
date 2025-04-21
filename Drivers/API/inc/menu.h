#ifndef MENU_H
#define MENU_H

#include "API_common.h"
#include "API_encoder.h"

#define MENU_DATA_UPDATE_TIME 1000U

typedef enum {
    STATE_INIT = 0,
    STATE_DISPLAY_DATA,
    STATE_CALIBRATION,
    STATE_RESET_CALIBRATION,
    STATE_CALIBRATION_TEMP,
    STATE_CALIBRATION_PRESS,
    STATE_CALIBRATION_HUM,
    STATE_OFFSET_TEMP,
    STATE_OFFSET_PRESS,
    STATE_OFFSET_HUM,
    STATE_EXIT,
} MenuState_t;

typedef struct {
    MenuState_t state_;
} stMenu;


void Menu_Init(void);
void Menu_Update(void);



#endif // MENU_H
