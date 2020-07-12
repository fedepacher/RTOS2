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
//extern SemaphoreHandle_t semaphore_app_ready;
/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
BaseType_t res;
QueueHandle_t queue;
//SemaphoreHandle_t semaphore_sep_ready;
/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/



bool_t init_separate_tasks(driver_t* Uart_driver){

	queue = xQueueCreate(POOL_TOTAL_BLOCKS, sizeof(mensaje_t));//esto lo quiero eliminar
	if(queue != NULL){
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
	mensaje_t *ptr_msj = NULL;
	while (1) {
		/*xQueueReceive(Uart_driver->onRxQueue, &ptr_msj, portMAX_DELAY);

		if(validate_String(ptr_msj)){	//chequea que no haya caracteres indeseados
			if(check_Crc(ptr_msj)){
				if(ptr_msj->msg[0] == m || ptr_msj->msg[0] == M){
					xQueueSend(queue, &ptr_msj, xBlockTime);
					//xSemaphoreGive(semaphore_sep_ready);
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

*/

		vTaskDelay(xPeriodicity);
	}
}

void app_receive_task(void* taskParmPtr) {
	driver_t *Uart_driver = (driver_t *) taskParmPtr;
	TickType_t xPeriodicity = 1 / portTICK_RATE_MS;	// Tarea periodica cada 1ms
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
	mensaje_t *ptr_msj = NULL;
	//mensaje_t *ptr_msj_armar = NULL;
	char *temp;
	//uint8_t index;
	char ptr_msj_armar[POOL_SIZE];
	while (1) {
		//ptr_msj_armar = (mensaje_t *)QMPool_get(&Uart_driver->Pool_memoria, 0);//pedimos memoria


		xQueueReceive(Uart_driver->onTxQueue, &ptr_msj, portMAX_DELAY);
		//if(ptr_msj_armar != NULL){
			gpioToggle(LED3);

			//temp = ptr_msj->msg;	//almaceno el registro donde empieza el puntero


			//calculo nuevo CRC
			volatile uint8_t seed_crc = crc8_init();
			uint8_t crc_l;
			uint8_t crc_h;
			/*seed_crc = crc8_calc(seed_crc, ptr_msj->msg, ptr_msj->lenght - 2); //-2 asi no incluye el crc de la cadena entrante
			crc_sep(seed_crc, &crc_h, &crc_l);



			uint8_t i;
			ptr_msj_armar[0] = '(';

			for(i = 0; i < ptr_msj->lenght - 2; i++){
				ptr_msj_armar[i + 1] = ptr_msj->msg[i];
			}
			//Uart_driver->ptr_pool_tx = ptr_msj;
			ptr_msj_armar[ptr_msj->lenght - 1] = crc_h;
			ptr_msj_armar[ptr_msj->lenght - 0] = crc_l;
			ptr_msj_armar[ptr_msj->lenght + 1] = ')';
			ptr_msj->lenght	+=	2; //le sumo 2 porque ahora tiene el ( y )
			//memcpy(Uart_driver->ptr_pool_tx->msg, ptr_msj_armar, ptr_msj->lenght);
			Uart_driver->txLen = ptr_msj->lenght;*/


			/**ptr_msj_armar->msg = '(';
			ptr_msj_armar->msg++;
			for(i = 0; i < ptr_msj->lenght - 2; i++){
				*ptr_msj_armar->msg = ptr_msj->msg[i];
				ptr_msj_armar->msg++;
			}
			*ptr_msj_armar->msg = crc_h;
			ptr_msj_armar->msg++;
			*ptr_msj_armar->msg = crc_l;
			ptr_msj_armar->msg++;
			*ptr_msj_armar->msg = ')';
			ptr_msj_armar->msg++;
			memcpy(Uart_driver->ptr_pool_tx->msg, ptr_msj_armar->msg, ptr_msj->lenght + 2);*/


			packetTX(Uart_driver);
			//QMPool_put(&Uart_driver->Pool_memoria, ptr_msj_armar);

		/*}else{
			char err[] = "No hay memoria";
			print_error(Uart_driver, err, strlen(err));
		}*/

		vTaskDelay(xPeriodicity);
	}
}

