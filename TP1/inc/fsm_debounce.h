/*
 * fsm_debounce.h
 *
 *  Created on: Jul 1, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_EJ_1_PARTE_1_INC_DRIVER_H_
#define PROGRAM_RTOS2_EJ_1_PARTE_1_INC_DRIVER_H_

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "semphr.h"
#include "types.h"


typedef enum
{
    STATE_BUTTON_UP,
    STATE_BUTTON_DOWN,
    STATE_BUTTON_FALLING,
    STATE_BUTTON_RISING
} fsmButtonState_t;


typedef struct
{
	gpioMap_t tecla;			//config

	TickType_t tiempo_medido;	//variables
	fsmButtonState_t fsmButtonState;//variables
	TickType_t tiempo_down;		//variables
	TickType_t tiempo_up;		//variables

	uint8_t contFalling  ;		//variables
	uint8_t contRising  ;		//variables
	bool_t btn_released;
	//QueueHandle_t queue_tec_pulsada;
} tecla_t;

/**
 * @brief  Error detected on FSM.
 * @param  config		gpio to be checked.
 * @retval none.
 */
void fsmButtonError( tecla_t* config );

/**
 * @brief  Initialize FSM.
 * @param  config		gpio to be initialized.
 * @param  i			index of gpio array.
 * @retval none.
 */
void fsmButtonInit( tecla_t* config, uint8_t i );

/**
 * @brief  FSM.
 * @param  config		gpio to be checked.
 * @retval true if btn was pressed otherwise ret false.
 */
bool_t fsmButtonUpdate( tecla_t* config );

/**
 * @brief  Button was pressed.
 * @param  config		gpio to be set.
 * @retval none.
 */
void buttonPressed( tecla_t* config );

/**
 * @brief  Button was pressed and released.
 * @param  config		gpio to be checked.
 * @retval true if btn was released otherwise ret false.
 */
bool_t buttonReleased( tecla_t* config );

#endif /* PROGRAM_RTOS2_EJ_1_PARTE_1_INC_DRIVER_H_ */
