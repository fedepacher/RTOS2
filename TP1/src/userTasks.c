/*
 * userTasks.c
 *
 *  Created on: Jun 30, 2020
 *      Author: fedepacher
 */

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"

/*=====[Inclusions of private function dependencies]=========================*/
#include "userTasks.h"
#include "driver.h"
#include "fsm_debounce.h"
#include "print_uart_error.h"
#include "separate_layer.h"
#include "application_layer.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/
tecla_t tec[N_TECLAS];
BaseType_t res;
/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/


void init_tasks() {

	static driver_t Uart_driver;

	Uart_driver.uartValue = UART_USB;
	Uart_driver.baudRate = 115200;
	Uart_driver.mutex = xSemaphoreCreateMutex();
	Uart_driver.start_timer = xSemaphoreCreateBinary();
	//Uart_driver.data_received = xSemaphoreCreateBinary();
	Uart_driver.onTxQueue = xQueueCreate(POOL_TOTAL_BLOCKS, sizeof(mensaje_t));
	Uart_driver.onRxQueue = xQueueCreate(POOL_TOTAL_BLOCKS, sizeof(mensaje_t));

	//inicializacion de uart driver
	if (driverInit(&Uart_driver) == FALSE) {
		char err[] = "ERROR Uart Driver Layer";
		print_error(&Uart_driver, err, strlen(err));
		while (1);
	}

	//inicializacion de capa de separacion
	if (init_separate_tasks(&Uart_driver) == FALSE) {
		char err[] = "ERROR Separate Layer";
		print_error(&Uart_driver, err, strlen(err));
		while (1);
	}

	//inicializacion de capa de aplicacion
	if (init_app_tasks(&Uart_driver) == FALSE) {
		char err[] = "ERROR Application Layer";
		print_error(&Uart_driver, err, strlen(err));
		while (1);
	}

	//inicializacion de teclas
	int i;
	for (i = 0; i < N_TECLAS; i++) {
		tec[0].tecla = teclas[0];
	}



	res = xTaskCreate(alive_task,         // Funcion de la tarea a ejecutar
				(const char *) "tarea_blinky", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, 	// Cantidad de stack de la tarea
				0,        		// Parametros de tarea
				tskIDLE_PRIORITY + 1,         	// Prioridad de la tarea
				0                         // Puntero a la tarea creada en el sistema
				);

	if (res == pdFAIL) {
		char err[] = "ERROR Blinky Task";
		print_error(&Uart_driver, err, strlen(err));
		while (1);
	}


}



void alive_task(void* taskParmPtr) {
	TickType_t xPeriodicity = 250 / portTICK_RATE_MS;	// Tarea periodica cada 250ms
	while (1) {
		gpioToggle(LEDB);
		vTaskDelay(xPeriodicity);
	}
}




