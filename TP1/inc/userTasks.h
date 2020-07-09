/*
 * userTasks.h
 *
 *  Created on: Jun 30, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_EJ_1_PARTE_1_INC_USERTASKS_H_
#define PROGRAM_RTOS2_EJ_1_PARTE_1_INC_USERTASKS_H_


/**
 * @brief  Initialize tasks a set uart.
 * @param  none
 * @retval none.
 */
void init_tasks();

/**
 * @brief  Blinky led task.
 * @param  none
 * @retval none.
 */
void alive_task( void* pvParameters );

#endif /* PROGRAM_RTOS2_EJ_1_PARTE_1_INC_USERTASKS_H_ */
