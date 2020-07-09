/*
 * error.h
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_TP1_INC_PRINT_UART_ERROR_H_
#define PROGRAM_RTOS2_TP1_INC_PRINT_UART_ERROR_H_

//#include "sapi.h"
#include "driver.h"

/**
 * @brief  Print error over uart.
 * @param  Uart_driver		uart to be sent.
 * @param  msg				msg to be printed
 * @param  lenght			msg's lenght
 * @retval none.
 */
void print_error(driver_t* Uart_driver, char * msg, uint8_t lenght);

#endif /* PROGRAM_RTOS2_TP1_INC_PRINT_UART_ERROR_H_ */
