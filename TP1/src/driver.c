/*=============================================================================
 * Copyright (c) 2019, Gonzalo Gontad <gonzalogontad@gmail.com>
 * All rights reserved.
 * License: MIT (see LICENSE.txt)
 * Date: 2019/09/29
 * Version: v1.0
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "driver.h"
#include "uartIRQ.h"
#include "types.h"
#include "print_uart_error.h"
#include "layer2.h"


/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/
//xTimerHandle timerHndl5SecTimeout;
BaseType_t res;
//QueueHandle_t separate_queue;
SemaphoreHandle_t time_expired_semaph;

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
mensaje_t *memory = NULL;	 // Segment of memory used to memory pool library
QMPool memoryPool;

/*=====[Prototypes (declarations) of private functions]======================*/
static void vTimerCallback5SecExpired(xTimerHandle pxTimer);
static void clean_Uart(driver_t* Uart_driver);
/*=====[Implementations of public functions]=================================*/

// Inicializacion de parte de transmision del driver
// Requiere que se le pase una variable del tipo driverConfig_t
// con los siguientes datos:
// 	selectedUart.uartValue = (por ej:UART_USB);
//	selectedUart.baudRate = (por ej: 115200);
bool_t driverInit( driver_t* Uart_driver )
{
	uartConfig(Uart_driver->uartValue,Uart_driver->baudRate);	// Inicializamos la UART

	Uart_driver->txCounter  = 0;


	Uart_driver->start_rx = FALSE;
	Uart_driver->stop_rx = FALSE;

	//reservamos pool de mem para rx
	memory = (mensaje_t *)pvPortMalloc(POOL_SIZE*sizeof(mensaje_t));
	// Create memory pool used on task and UART
	QMPool_init(&memoryPool, (mensaje_t *)memory, POOL_SIZE*sizeof(mensaje_t), PACKET_SIZE);

	if(memory == NULL){
		char err[] = "ERROR Dinamic memory allocation\r\n";
		print_error(Uart_driver, err, strlen(err));
		while(1);
	}

	memoryBlock = &memoryPool;

	//reservamos pool de mem para rx
		memory = (mensaje_t *)pvPortMalloc(POOL_SIZE*sizeof(mensaje_t));
		// Create memory pool used on task and UART
		QMPool_init(&memoryPool, (mensaje_t *)memory, POOL_SIZE*sizeof(mensaje_t), PACKET_SIZE);

		if(memory == NULL){
			char err[] = "ERROR Dinamic memory allocation\r\n";
			print_error(Uart_driver, err, strlen(err));
			while(1);
		}

		memoryBlock_aux = &memoryPool;


	Uart_driver->rxLen = 0;
	Uart_driver->txLen = 0;


	// Configuramos el cb de rx
	if (( rxInterruptEnable( Uart_driver )))
	{}
	else
	{
		return FALSE;
	}

	//configuramos timer de reseteo de cadena
	Uart_driver->timerHndl5SecTimeout = xTimerCreate("timer", /* name */
												pdMS_TO_TICKS(5000), /* period/time */
												pdFALSE, /* auto reload */
												(void*) 1, /* timer ID */
												vTimerCallback5SecExpired); /* callback */

	if (Uart_driver->timerHndl5SecTimeout == NULL) {
		char err[] = "ERROR timer\r\n";
		print_error(Uart_driver, err, strlen(err));
		while (1);
	}

	time_expired_semaph = xSemaphoreCreateBinary();

	if (time_expired_semaph == pdFAIL) {
		char err[] = "ERROR Queue Init\r\n";
		print_error(Uart_driver, err, strlen(err));
		while (1);
	}

	res = xTaskCreate(reset_uart_task,         // Funcion de la tarea a ejecutar
			(const char *) "tarea_reset_uart", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, 	// Cantidad de stack de la tarea
			(void*) Uart_driver,        		// Parametros de tarea
			tskIDLE_PRIORITY + 1,         	// Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
	);

	if (res == pdFAIL) {
		char err[] = "ERROR Reset Uart Task\r\n";
		print_error(Uart_driver, err, strlen(err));
		while (1);
	}

	// NO OLVIDAR: Habilitar interrupciones
	uartInterrupt(Uart_driver -> uartValue, TRUE);

	return TRUE;

}

//expira el timer
static void vTimerCallback5SecExpired(xTimerHandle pxTimer) {
	xSemaphoreGive(time_expired_semaph);
}

//reseteae los valores de uart para reiniciar la recepcion ya que paso el tiempo de espera
void reset_uart_task(void* taskParmPtr) {
	driver_t *Uart_driver = (driver_t *) taskParmPtr;
	TickType_t xPeriodicity = 1 / portTICK_RATE_MS;	// Tarea periodica cada 1ms

	while (1) {
		xSemaphoreTake(time_expired_semaph, portMAX_DELAY);
		taskENTER_CRITICAL();
		gpioToggle(LED1);
		//expiro el tiempo de ingreso de datos, se resetea el sistema
		if (Uart_driver->start_rx == TRUE && Uart_driver->stop_rx == FALSE) {
			clean_Uart(Uart_driver);
			char err[] = "Supero tiempo";
			print_error(Uart_driver, err, strlen(err));
		}
		taskEXIT_CRITICAL();
		vTaskDelay(xPeriodicity);
	}
}


void packetTX(driver_t* Uart_driver)		//, char *mensaje )
{
	taskENTER_CRITICAL();  //no permito que se modifique txcounter
	if (Uart_driver->txCounter == 0) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
	{
		txInterruptEnable(Uart_driver);
	}
	taskEXIT_CRITICAL();
	uartSetPendingInterrupt(Uart_driver->uartValue);
}

static void clean_Uart(driver_t* Uart_driver) {
	//QMPool_put(&Uart_driver->Pool_memoria, Uart_driver->ptr_pool_rx);		//libero pool de memoria
	Uart_driver->rxCounter = 0;
	Uart_driver->rxLen = 0;
	Uart_driver->start_rx = FALSE;
	Uart_driver->stop_rx = FALSE;
}
