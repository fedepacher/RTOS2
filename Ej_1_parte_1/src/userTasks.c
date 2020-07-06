/*
 * userTasks.c
 *
 *  Created on: Jun 30, 2020
 *      Author: fedepacher
 */


#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "perifericos.h"
#include "userTasks.h"
#include "driver.h"
#include "fsm_debounce.h"
#include "queue.h"
#include "qmpool.h"

#define N_ELEM_COLA		10

QueueHandle_t cola_1;
tecla_t tec[N_TECLAS];
BaseType_t res;

char mensaje[20];


void init_tasks(){

	static driver_t Uart_driver;

	Uart_driver.uartValue = UART_USB;
	Uart_driver.baudRate = 115200;
	Uart_driver.mutex = xSemaphoreCreateMutex();
	Uart_driver.permiso = xSemaphoreCreateBinary();

	if ( driverInit( &Uart_driver ) == FALSE )
	{
			//atender error
	}

	int i;
	for( i = 0; i < N_TECLAS; i++ ){
		tec[0].tecla = teclas[0];
	}

	cola_1 = xQueueCreate(N_ELEM_COLA, sizeof(mensaje_t));

	if(cola_1 != NULL){
		res = xTaskCreate(
				tarea_a,                     	// Funcion de la tarea a ejecutar
				( const char * )"tarea_a",   // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
				0,        		// Parametros de tarea
				tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
				0                           	// Puntero a la tarea creada en el sistema
			);

		if(res == pdFAIL){
			//atender error
			while(1);
		}

		res = 	xTaskCreate(
					tarea_b,                     // Funcion de la tarea a ejecutar
					( const char * )"tarea_b_tec1",   // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
					( void* )tec,                        // Parametros de tarea
					tskIDLE_PRIORITY+2,         // Prioridad de la tarea
					0                           // Puntero a la tarea creada en el sistema
			);

		if(res == pdFAIL){
					//atender error
			while(1);
		}


		res = 	xTaskCreate(
					tarea_c,                     // Funcion de la tarea a ejecutar
			        ( const char * )"tarea_c",   // Nombre de la tarea como String amigable para el usuario
			        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
			        ( void* )&Uart_driver,                        // Parametros de tarea
			        tskIDLE_PRIORITY+2,         // Prioridad de la tarea
			        0                           // Puntero a la tarea creada en el sistema
			    );

		if(res == pdFAIL){
					//atender error
			while(1);
		}
	}
	else{
		//atender error
		while(1);
	}


}

void tarea_a( void* taskParmPtr ){

	TickType_t xPeriodicity =  1000 / portTICK_RATE_MS;		// Tarea periodica cada 1000 ms
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
	char msj[] = "LED ON";
	while(1){

		gpioToggle(LEDB);
		if(gpioRead(LEDB)){

			xQueueSend(cola_1, msj, xBlockTime);
		}

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}

void tarea_b( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tecla_t* config = (tecla_t*) taskParmPtr;
	TickType_t diferencia  = 0;
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
	int i = 0;
	for(i = 0; i < N_TECLAS; i++){
		fsmButtonInit( &config[i], i);
	}
	char msj[20];
	// ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
    	for(i = 0; i < N_TECLAS; i++){
    		if(fsmButtonUpdate( &config[i] )){
    			sprintf (msj, "TEC%d T%dms", config[i].tecla+1-TEC1, config[i].tiempo_medido);
    			xQueueSend(cola_1, msj, xBlockTime);
    		}
    	}


		vTaskDelay( 1 / portTICK_RATE_MS );
    }
}

void tarea_c( void* taskParmPtr ){
	//periferico_t* config = (periferico_t*) taskParmPtr;
	driver_t *Uart_driver;
	Uart_driver = ( driver_t * )taskParmPtr;
	while(1){

		xQueueReceive(cola_1, &mensaje, portMAX_DELAY);
		Uart_driver->txLen = strlen(mensaje);
		packetTX( Uart_driver , mensaje );
		vTaskDelay(1 / portTICK_RATE_MS);
	}
}


void packetTX( driver_t* Uart_driver, char *mensaje )
{

    //xQueueSend( Uart_driver->onTxQueue, &mensaje, portMAX_DELAY ); //Envio a la cola de transmision el blocke a transmitir
    taskENTER_CRITICAL();  //no permito que se modifique txcounter
    if ( Uart_driver->txCounter == 0 ) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
    {
        txInterruptEnable( Uart_driver );
    }
    taskEXIT_CRITICAL();
    uartSetPendingInterrupt( Uart_driver->uartValue );
}

