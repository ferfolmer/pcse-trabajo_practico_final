#ifndef API_DEBOUNCE_H
#define API_DEBOUNCE_H

#include "API_delay.h"
#include "API_common.h"


typedef enum{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RAISING,
} debounceState_t;


typedef struct
{
    gpio_t              *gpio_;            
    delay_t             *delay_;         
    debounceState_t     state_;
    debounceState_t     validState_;     
    bool_t              isButtonPressed_;
    bool_t              isInit;          
} stDebounce;

 /**
  * @brief inicializa la MEF (Maquina de estado Finita) que implementa un 
  * antirrebote (debouncer) del boton de usuario
  * 
  * @param button puntero a la estructura a ser debounceada
  * @param pin 
  * @param delay 
  */
void debounceFSM_init(stDebounce *button, gpio_t *pin, delay_t *delay);

 /**
  * @brief actualiza periodicamente la MEF a la frecuencia del loop principal 
  * 
  * @param button puntero a la estructura a ser debounceada
  */
void debounceFSM_update(stDebounce *button);

/* La función debounceFSM_readKey debe leer una variable interna del módulo y devolver
 true o false si la tecla fue presionada.  Si devuelve true, debe resetear
 (poner en false) el estado de la variable.*/
/**
 * @brief lee el estado de la tecla y lo devuelve. Si la tecla fue presionada
 * 
 * @param button 
 * @return bool_t 
 */
 bool_t debounceFSM_readKey(stDebounce *button);

debounceState_t debounceFSM_getState(stDebounce *button);



#endif //API_DEBOUNCE_H

