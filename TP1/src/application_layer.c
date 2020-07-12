/*
 * application_layer.c
 *
 *  Created on: Jul 9, 2020
 *      Author: fedepacher
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "sapi.h"
#include "types.h"
#include "crc8.h"
#include "print_uart_error.h"
#include "application_layer.h"
#include "extra_func.h"
#include "layer2.h"
//#include "separate_layer.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
BaseType_t res;
//SemaphoreHandle_t semaphore_app_ready;
/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/



bool_t init_app_tasks(driver_t* Uart_driver){


		res = xTaskCreate(app_task,     // Funcion de la tarea a ejecutar
				(const char *) "app_task", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, 	// Cantidad de stack de la tarea
				(void*) Uart_driver,        		// Parametros de tarea
				tskIDLE_PRIORITY + 1,         	// Prioridad de la tarea
				0                         // Puntero a la tarea creada en el sistema
		);

		if (res == pdFAIL) {
			return FALSE;
		}


	return TRUE;
}


//analisis de cadena de caracteres
void app_task(void* taskParmPtr) {
	driver_t *Uart_driver = (driver_t *) taskParmPtr;
	TickType_t xPeriodicity = 1 / portTICK_RATE_MS;	// Tarea periodica cada 1ms
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
	layer2_t ptr_msj;
	uint8_t result = 0;
	while (1) {
		xQueueReceive(Uart_driver->onRxQueue, &ptr_msj, portMAX_DELAY);

		if(validate_String(&ptr_msj)){
			switch (ptr_msj.c) {
			case m:
				result = lowercase_String(ptr_msj.dato);
				break;
			case M:
				result = uppercase_String(ptr_msj.dato);
				break;
			default:
				result = 0;	//si viene ac es porque se no tenia la m o M
				break;
			}
		}

		if(result){
			layer2_msg_gen(&ptr_msj, Uart_driver);
		}
		layer2_destructor(&ptr_msj, HANDLER_MODE_OFF);
		vTaskDelay(xPeriodicity);
	}
}
