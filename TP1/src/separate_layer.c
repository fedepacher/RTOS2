/*
 * separate_string.c
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#include "separate_layer.h"

#include "extra_func.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "sapi.h"
#include "types.h"
#include "crc8.h"
#include "print_uart_error.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/
extern SemaphoreHandle_t semaphore_app_ready;
/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
BaseType_t res;
//QueueHandle_t queue;
SemaphoreHandle_t semaphore_sep_ready;
/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/



bool_t init_separate_tasks(driver_t* Uart_driver){

	//inicializacion de cola
	//queue = xQueueCreate(POOL_TOTAL_BLOCKS, sizeof(mensaje_t));
	semaphore_sep_ready = xSemaphoreCreateBinary();
	if(semaphore_sep_ready != NULL){
		res = xTaskCreate(separate_string_task,     // Funcion de la tarea a ejecutar
					(const char *) "separate_string_task", // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE * 2, 	// Cantidad de stack de la tarea
					(void*) Uart_driver,        		// Parametros de tarea
					tskIDLE_PRIORITY + 1,         	// Prioridad de la tarea
					0                         // Puntero a la tarea creada en el sistema
			);

		if (res == pdFAIL) {
				return FALSE;
			}

		res = xTaskCreate(app_receive_task,     // Funcion de la tarea a ejecutar
				(const char *) "app_receive_task", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, 	// Cantidad de stack de la tarea
				(void*) Uart_driver,        		// Parametros de tarea
				tskIDLE_PRIORITY + 1,         	// Prioridad de la tarea
				0                         // Puntero a la tarea creada en el sistema
		);

		if (res == pdFAIL) {
			return FALSE;
		}
	}
	else{
		return FALSE;
	}

	return TRUE;

}


//analisis de cadena de caracteres
void separate_string_task(void* taskParmPtr) {
	driver_t *Uart_driver = (driver_t *) taskParmPtr;
	TickType_t xPeriodicity = 1 / portTICK_RATE_MS;	// Tarea periodica cada 1ms
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );

	while (1) {
		xSemaphoreTake(Uart_driver->data_received, portMAX_DELAY);

		//Uart_driver->ptr_pool_tx = (mensaje_t *)QMPool_get(&Uart_driver->Pool_memoria, 0);//pedimos memoria
		if(validate_String(Uart_driver->ptr_pool_rx)){	//chequea que no haya caracteres indeseados
			if(check_Crc(Uart_driver->ptr_pool_rx)){

				/*switch (Uart_driver->ptr_pool_rx->msg[0]) {
				case m:
					result = low_String(Uart_driver->ptr_pool_rx);
					break;
				case M:
					result = high_String(Uart_driver->ptr_pool_rx);
					break;
				default:

					break;
				}
				if(result){
					Uart_driver->ptr_pool_tx = Uart_driver->ptr_pool_rx;
					Uart_driver->txLen = Uart_driver->ptr_pool_rx->lenght;

					gpioToggle(LED3);
					packetTX(Uart_driver);
				}*/
				if(Uart_driver->ptr_pool_rx->msg[0] == m || Uart_driver->ptr_pool_rx->msg[0] == M){
					//xQueueSend(queue, &Uart_driver->ptr_pool_rx, xBlockTime);
					xSemaphoreGive(semaphore_sep_ready);
				}
				else{
					char err[] = "Falta la M o n";
					print_error(Uart_driver, err, strlen(err));
				}
			}
			else{
				char err[] = "CRC invalidos";
				print_error(Uart_driver, err, strlen(err));
			}
		}
		else{
			char err[] = "Hay caracteres invalidos";
			print_error(Uart_driver, err, strlen(err));
		}



		vTaskDelay(xPeriodicity);
	}
}

void app_receive_task(void* taskParmPtr) {
	driver_t *Uart_driver = (driver_t *) taskParmPtr;
	TickType_t xPeriodicity = 1 / portTICK_RATE_MS;	// Tarea periodica cada 1ms
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );

	while (1) {
		xSemaphoreTake(semaphore_app_ready, portMAX_DELAY);
		gpioToggle(LED3);

		Uart_driver->ptr_pool_tx = Uart_driver->ptr_pool_rx;
		Uart_driver->txLen = Uart_driver->ptr_pool_rx->lenght;
		packetTX(Uart_driver);
		Uart_driver->rxLen = 0;

		vTaskDelay(xPeriodicity);
	}
}

