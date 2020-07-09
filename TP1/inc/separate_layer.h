/*
 * separate_string.h
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_TP1_INC_SEPARATE_LAYER_H_
#define PROGRAM_RTOS2_TP1_INC_SEPARATE_LAYER_H_

#include "driver.h"

/**
 * @brief  Initialize task.
 * @param  Uart_driver		uart to be used.
 * @retval true if ok otherwise false.
 */
bool_t init_separate_tasks(driver_t* Uart_driver);

/**
 * @brief  Check if received frame was ok and check crc, if it is ok send packet to app layer
 * @param  taskParmPtr		uart to be checked.
 * @retval none.
 */
void separate_string_task(void* taskParmPtr);

/**
 * @brief  receive packet to be sent to uart
 * @param  taskParmPtr		uart to be checked.
 * @retval none.
 */
void app_receive_task(void* taskParmPtr);

#endif /* PROGRAM_RTOS2_TP1_INC_SEPARATE_LAYER_H_ */
