/*
 * error.c
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#include "print_uart_error.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

void print_error(driver_t* Uart_driver, char * msg, uint8_t lenght){
	//Uart_driver->ptr_pool_rx = (mensaje_t *)QMPool_get(&Uart_driver->Pool_memoria, 0);//pedimos memoria
	Uart_driver->ptr_pool_tx->msg = msg;
	Uart_driver->txLen = lenght;
	packetTX(Uart_driver);
	//QMPool_put( &Uart_driver->Pool_memoria, Uart_driver->ptr_pool_tx );
	//Uart_driver->txLen = 0;
}

