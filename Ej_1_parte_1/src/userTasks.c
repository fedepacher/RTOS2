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

QueueHandle_t cola_1;
periferico_t periferico[1];
tecla_t teclas[CANT];
BaseType_t res;

char mensaje[20];

static char* itoa(int value, char* result, int base);


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

	periferico[0].periferico = LEDB;
	//periferico[0].msj = "";
	periferico[0].mutex = xSemaphoreCreateMutex();


	teclas[0].tecla.periferico = TEC1;
	teclas[0].queue_tec_pulsada	= xQueueCreate(1,sizeof(TickType_t));
	teclas[0].tecla.mutex = xSemaphoreCreateMutex();
	char text0[] = "TEC1 T=";
	memcpy(&teclas[0].tecla.msj, text0, strlen(text0));


	teclas[1].tecla.periferico = TEC2;
	teclas[1].queue_tec_pulsada	= xQueueCreate(1,sizeof(TickType_t));
	teclas[1].tecla.mutex =  xSemaphoreCreateMutex();
	char text1[] = "TEC2 T=";
	memcpy(&teclas[1].tecla.msj, text1, strlen(text1));


	cola_1 = xQueueCreate(10, sizeof(mensaje_t));

	if(cola_1 != NULL){
		res = xTaskCreate(
				tarea_a,                     	// Funcion de la tarea a ejecutar
				( const char * )"tarea_a",   // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
				&periferico[0],        		// Parametros de tarea
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
					( void* )&teclas[0],                        // Parametros de tarea
					tskIDLE_PRIORITY+2,         // Prioridad de la tarea
					0                           // Puntero a la tarea creada en el sistema
			);

		if(res == pdFAIL){
					//atender error
			while(1);
		}

		res = 	xTaskCreate(
					tarea_b,                     // Funcion de la tarea a ejecutar
					( const char * )"tarea_b_tec2",   // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
					( void* )&teclas[1],                        // Parametros de tarea
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

		res = xTaskCreate(
			    	tarea_tecla,                     // Funcion de la tarea a ejecutar
			        ( const char * )"tarea_tecla1",  // Nombre de la tarea como String amigable para el usuario
			        configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
					&teclas[0],           // Parametros de tarea
			        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
			        0                           	// Puntero a la tarea creada en el sistema
			    );

		if(res == pdFAIL){
					//atender error
			while(1);
		}

		res = xTaskCreate(
			    	tarea_tecla,                     // Funcion de la tarea a ejecutar
			        ( const char * )"tarea_tecla2",  // Nombre de la tarea como String amigable para el usuario
			        configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
					&teclas[1],           // Parametros de tarea
			        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
			        0                           	// Puntero a la tarea creada en el sistema
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

void tarea_tecla( void* taskParmPtr )
{
	tecla_t* config = (tecla_t*) taskParmPtr;

	fsmButtonInit( config );

	while( 1 )
	{
		fsmButtonUpdate( config );
	 	vTaskDelay( 1 / portTICK_RATE_MS );
	}
}




void tarea_a( void* taskParmPtr ){

	periferico_t* config = (periferico_t*) taskParmPtr;
	TickType_t xPeriodicity =  1000 / portTICK_RATE_MS;		// Tarea periodica cada 1000 ms
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
	char msj[] = "LED ON";
	while(1){

		gpioToggle(config->periferico);
		if(gpioRead(config->periferico)){
			memcpy(&config->msj, msj, strlen(msj));
			xSemaphoreTake(config->mutex , portMAX_DELAY);
			xQueueSend(cola_1, &config->msj, xBlockTime);
			xSemaphoreGive(config->mutex);
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
	char msj[20];
	char msj_diferencia[10];
    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
    	memset(msj, '\0', sizeof(msj));
    	memset(msj_diferencia, '\0', sizeof(msj_diferencia));
    	xQueueReceive( config->queue_tec_pulsada , &diferencia,  portMAX_DELAY );			// Esperamos tecla

    	memcpy(msj, &config->tecla.msj, strlen((char *)&config->tecla.msj));

    	itoa(diferencia, msj_diferencia , 10);
    	strcat((char *)msj, (char *)msj_diferencia);

    	xSemaphoreTake( config->tecla.mutex , portMAX_DELAY );			//abrir seccion critica

		xQueueSend(cola_1, msj, xBlockTime);

		xSemaphoreGive( config->tecla.mutex );							//cerrar seccion critica
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

static char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}
