
/*=====[Inclusions of private function dependencies]=========================*/
#include "uartIRQ.h"
#include "semphr.h"
#include "types.h"
#include "crc8.h"
#include "extra_func.h"
#include "layer2.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/
void onTxCallback( void* );
void onRxCallback( void* );
/*=====[Implementations of public functions]=================================*/


bool_t rxInterruptEnable( driver_t* Uart_driver )
{
	switch( Uart_driver-> uartValue)
	{
		case UART_GPIO:
		case UART_USB:
		case UART_232:
		{
			uartCallbackSet( Uart_driver->uartValue, UART_RECEIVE, onRxCallback, (void*)Uart_driver );
			break;
		}
		default:
		{
			return (FALSE); // No se eligio una UART correcta
		}
	}

	return TRUE;
}

//txInterruptEnable(driverConfig_t* Uart_driver) Habilita la interrupcion de la UART para transmision
//indicada por selectedUart
 bool_t txInterruptEnable( driver_t* Uart_driver )
 {
	 switch( Uart_driver-> uartValue)
	{
		case UART_GPIO:
		case UART_USB:
		case UART_232:
		{
			uartCallbackSet( Uart_driver->uartValue, UART_TRANSMITER_FREE, onTxCallback, (void*)Uart_driver );
			break;
		}
		default:
		{
			return (FALSE); // No se eligio una UART correcta
		}
	}

	return TRUE;

 }
/*=====[Implementations of interrupt functions]==============================*/

// Callback para la recepcion
 void onRxCallback( void *param )
 {
	 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	// volatile uint8_t seed_crc;
	 char c;
	 // Elegir los datos que necesita el cb

	 driver_t* Uart_driver = (driver_t*)param;
	 layer2_t message;

	 //abrir seccion critica
	 xSemaphoreTakeFromISR( Uart_driver->mutex 	  , &xHigherPriorityTaskWoken );

	 // Si llegan mas bytes de los que puedo guardar, no los leo.
	 if ( Uart_driver->rxLen < PACKET_SIZE )
	 {
		 // Leemos el byte de la UART seleccionado
		 c = uartRxRead( Uart_driver->uartValue );
		 Uart_driver->rxCounter++;	//incrementamos contador de rx
		 if(c == SOFRAME && !Uart_driver->start_rx){	//chequeo inicio de trama, condicion necesaria que que haya ( y start_rx = false
			 Uart_driver->start_rx = TRUE;			//inicio de trama ok
			 Uart_driver->stop_rx = FALSE;

			 Uart_driver->dataRx = (mensaje_t *)QMPool_get_FROM_ISR(memoryBlock, 0);//pedimos memoria

			 xTimerStartFromISR(Uart_driver->timerHndl5SecTimeout, &xHigherPriorityTaskWoken);
		 }
		 else{
			 if(c == EOFRAME & Uart_driver->start_rx)
			 {
				 uint8_t crc_l;
				 uint8_t crc_h;
				 xTimerStopFromISR(Uart_driver->timerHndl5SecTimeout, &xHigherPriorityTaskWoken);
				 Uart_driver->start_rx = FALSE;				//mensaje recibido ok
				 Uart_driver->stop_rx = TRUE;
				 Uart_driver->dataRx[Uart_driver->rxLen++] = '\0';
			 }
			 else{
				 if(Uart_driver->start_rx){
					 Uart_driver->dataRx[Uart_driver->rxLen++] = c;
					 xTimerResetFromISR(Uart_driver->timerHndl5SecTimeout, &xHigherPriorityTaskWoken);
				 }
			 }
		 }

		 if(Uart_driver->stop_rx){
			 Uart_driver->rxCounter = 0;
			 Uart_driver->stop_rx = FALSE;
			 Uart_driver -> rxLen = 0;
			 layer2_constructor(&message, HANDLER_MODE_ON);//Uart_driver->message);	//creo instancia de layer2
			 layer2_frame_separation(&message, Uart_driver->dataRx);

			 //libero memoria de dataRx
			 QMPool_put_FROM_ISR(memoryBlock, Uart_driver->dataRx );
			 Uart_driver->dataRx = NULL;

			 if(layer2_check_crc(&message) == 1)
				 xQueueSendFromISR(Uart_driver->onRxQueue, &message, &xHigherPriorityTaskWoken);

		 }
	 }
	 else
	 {
		 Uart_driver->rxCounter = 0;
		 Uart_driver->start_rx = FALSE;
		 Uart_driver->stop_rx = FALSE;
		 Uart_driver->rxLen = 0;
		 QMPool_put_FROM_ISR(memoryBlock, Uart_driver->dataRx );
		 Uart_driver->dataRx = NULL;
	 }

	 xSemaphoreGiveFromISR( Uart_driver->mutex , &xHigherPriorityTaskWoken);
	 //cerrar seccion critica

	 portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 }
//	uartSendDataCRC transmite los bloques que llegan por una cola y los envia por la UART seleccionada
//	Todas las callback de interrupcion de UART llaman a esta función para hacer la transmisión.
 void onTxCallback( void*param )
 {
	 driver_t* Uart_driver;
	 UBaseType_t uxSavedInterrumptStatus;
	 BaseType_t xTaskWokenByReceive = pdFALSE;

	 Uart_driver = ( driver_t* ) param;

	 // Armo una seccion critica para proteger tx_counter
	 uxSavedInterrumptStatus = taskENTER_CRITICAL_FROM_ISR();

	 // Si el contador es 0 -> puedo tomar un paquete de la cola
	 /*if ( Uart_driver -> txCounter == 0)
	 {
		 Uart_driver -> txLen = Uart_driver -> rxLen-1; //

		 //Uart_driver -> txCounter++;
	 }*/

	 if ( Uart_driver-> txCounter < Uart_driver->txLen)
	 { 	// Transmision
		uartTxWrite(Uart_driver->uartValue, Uart_driver->dataTx[Uart_driver->txCounter]);
		Uart_driver -> txCounter ++;
	 }

	 if ( Uart_driver-> txCounter == Uart_driver->txLen)
	 {
		 uartTxWrite(Uart_driver->uartValue, '\r');
		 uartTxWrite(Uart_driver->uartValue, '\n');
		 uartCallbackClr(Uart_driver->uartValue, UART_TRANSMITER_FREE); // Deshabilito la interrupcion de Tx
		 //xTimerStartFromISR(Uart_driver->onTxTimeOut, &xTaskWokenByReceive); // Inicio el timer de timeout Tx
		 //Uart_driver->ptr_pool_tx->lenght = 0;
		 Uart_driver->txLen = 0;
		 Uart_driver-> txCounter = 0;
		 //free memory
		 //layer2_desconstructor(&message, HANDLER_MODE_ON);

		 QMPool_put_FROM_ISR(memoryBlock, Uart_driver->dataTx);
		 //Uart_driver->dataRx = NULL;

	 }

	 taskEXIT_CRITICAL_FROM_ISR( uxSavedInterrumptStatus );

	 // Evaluamos el pedido de cambio de contexto necesario
	 if ( xTaskWokenByReceive != pdFALSE )
	 {
		 taskYIELD();
	 }

 }

