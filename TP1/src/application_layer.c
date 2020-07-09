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
#include "separate_layer.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/
extern QueueHandle_t queue;;
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
	mensaje_t *ptr_msj = NULL;
	bool_t result = FALSE;
	while (1) {
		xQueueReceive(queue, &ptr_msj, portMAX_DELAY);
		//xSemaphoreTake(semaphore_sep_ready, portMAX_DELAY);
		result = FALSE;
		switch (ptr_msj->msg[0]) {
		case m:
			result = lowercase_String(ptr_msj);
			break;
		case M:
			result = uppercase_String(ptr_msj);
			break;
		default:

			break;
		}
		if(result){
			//xSemaphoreGive(semaphore_app_ready);
			xQueueSend(Uart_driver->onTxQueue, &ptr_msj, xBlockTime);
		}

		vTaskDelay(xPeriodicity);
	}
}
