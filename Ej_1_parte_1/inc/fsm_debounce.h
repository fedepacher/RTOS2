/*
 * fsm_debounce.h
 *
 *  Created on: Jul 1, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_EJ_1_PARTE_1_INC_DRIVER_H_
#define PROGRAM_RTOS2_EJ_1_PARTE_1_INC_DRIVER_H_

#include "FreeRTOSConfig.h"
#include "perifericos.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "semphr.h"

#define CANT	2
typedef enum
{
    STATE_BUTTON_UP,
    STATE_BUTTON_DOWN,
    STATE_BUTTON_FALLING,
    STATE_BUTTON_RISING
} fsmButtonState_t;


typedef struct
{
	periferico_t tecla;			//config

	TickType_t tiempo_medido;	//variables
	fsmButtonState_t fsmButtonState;//variables
	TickType_t tiempo_down;		//variables
	TickType_t tiempo_up;		//variables

	uint8_t contFalling  ;		//variables
	uint8_t contRising  ;		//variables
	QueueHandle_t queue_tec_pulsada;
} tecla_t;

void fsmButtonError( tecla_t* config );
void fsmButtonInit( tecla_t* config );
void fsmButtonUpdate( tecla_t* config );
void buttonPressed( tecla_t* config );
void buttonReleased( tecla_t* config );

#endif /* PROGRAM_RTOS2_EJ_1_PARTE_1_INC_DRIVER_H_ */
