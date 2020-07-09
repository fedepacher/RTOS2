/*
 * application_layer.h
 *
 *  Created on: Jul 9, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_TP1_INC_APPLICATION_LAYER_H_
#define PROGRAM_RTOS2_TP1_INC_APPLICATION_LAYER_H_


#include "driver.h"

/**
 * @brief  Initialize task.
 * @param  Uart_driver		uart to be used.
 * @retval true if ok otherwise false.
 */
bool_t init_app_tasks(driver_t* Uart_driver);

/**
 * @brief  Check if received frame was ok and check crc, if it is ok convert to uppercase letters if M was received, if m lowercase
 * @param  taskParmPtr		uart to be checked.
 * @retval none.
 */
void app_task(void* taskParmPtr);


#endif /* PROGRAM_RTOS2_TP1_INC_APPLICATION_LAYER_H_ */
