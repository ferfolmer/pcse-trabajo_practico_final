#ifndef API_ENCODER_H
#define API_ENCODER_H

#include "API_common.h"
#include "API_debounce.h"

typedef enum {
    ENCODER_OK = 0,
    ENCODER_ERROR,
} Encoder_Status_t;

typedef enum {
    ENCODER_CW = 0,
    ENCODER_CCW,
    ENCODER_DIR_NONE
} Encoder_Direction_t;

Encoder_Status_t Encoder_Init(void);
void Encoder_Update(void);
Encoder_Direction_t Encoder_GetDirection(void);
bool_t Encoder_IsPressed(void);

#endif // API_ENCODER_H
