#ifndef UARTIRQ_H_
#define UARTIRQ_H_

#include "driver.h"

/**
 * @brief  Set uart tx callback function.
 * @param  Uart_driver		uart to be used.
 * @retval true if ok otherwise false.
 */
bool_t txInterruptEnable( driver_t* );

/**
 * @brief  Set uart rx callback function.
 * @param  Uart_driver		uart to be used.
 * @retval true if ok otherwise false.
 */
bool_t rxInterruptEnable( driver_t* );

/**
 * @brief  Callback function.
 * @param  param
 * @retval none.
 */
void onTxTimeOutCallback(  void *param );

/**
 * @brief  Callback function.
 * @param  param
 * @retval none.
 */
void onRxTimeOutCallback(  void *param );

#endif
