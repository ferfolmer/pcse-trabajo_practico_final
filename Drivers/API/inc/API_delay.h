#ifndef API_DELAY_H
#define API_DELAY_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief 
 * 
 */
typedef struct {
	tick_t startTime;
	tick_t duration;
	bool_t running;
} delay_t;

/**
 * @brief carga el valor de duracion de retardo en la estructura delay, 
 *	inicializa `running` en false
 * 
 * @param delay referencia a la estructura delay
 * @param duration duracion del retardo
 */
void delayInit(delay_t * delay, tick_t duration);

/**
 * @brief verifica el estado del flag `running` de la estructura recibida
 * 
 * @param delay referencia a la estructura delay
 * @return true si se cumplio el tiempo en duration
 * @return false en caso contrario
 */
bool_t delayRead(delay_t * delay);

/**
 * @brief cambia el tiempo de duracion de un delay existente
 * 
 * @param delay referencia a la estructura delay
 * @param duration nueva duracion del retardo
 */
void delayWrite(delay_t * delay, tick_t duration);

/**
 * @brief verifica si el delay esta corriendo
 * 
 * @param delay referencia a la estructura delay
 * @return true si el delay esta corriendo aun
 * @return false en caso contrario
 */
bool_t delayIsRunning(delay_t * delay);

#endif //API_DELAY_H
