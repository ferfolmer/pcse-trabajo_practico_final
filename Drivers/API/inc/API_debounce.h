#ifndef API_DEBOUNCE_H
#define API_DEBOUNCE_H

#include <stdbool.h>
#include "API_delay.h"



/**
 * @brief inicializa la MEF (Maquina de estado Finita) que implementa un 
 * antirrebote (debouncer) del boton de usuario    
 * 
 */
void debounceFSM_init(void);

/**
 * @brief actualiza periodicamente la MEF a la frecuencia del loop principal
 * 
 */
void debounceFSM_update(void);

/* La función readKey debe leer una variable interna del módulo y devolver
 true o false si la tecla fue presionada.  Si devuelve true, debe resetear
 (poner en false) el estado de la variable.*/
bool_t readKey(void);



#endif //API_DEBOUNCE_H

