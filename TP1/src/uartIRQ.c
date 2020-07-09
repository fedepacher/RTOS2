

/*=====[Inclusions of private function dependencies]=========================*/
#include "uartIRQ.h"
#include "semphr.h"
#include "types.h"
#include "crc8.h"
#include "extra_func.h"
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
	 volatile uint8_t seed_crc;
	 char c;
	 // Elegir los datos que necesita el cb

	 driver_t* Uart_driver = param;

	 //abrir seccion critica
	 xSemaphoreTakeFromISR( Uart_driver->mutex 	  , &xHigherPriorityTaskWoken );

	 // Si llegan mas bytes de los que puedo guardar, no los leo.
	 if ( Uart_driver->rxLen < LENGHT_MAX )
	 {
		 // Leemos el byte de la UART seleccionado
		 c = uartRxRead( Uart_driver->uartValue );
		 Uart_driver->rxCounter++;	//incrementamos contador de rx
		 if(c == '(' && !Uart_driver->start_rx){	//chequeo inicio de trama, condicion necesaria que que haya ( y start_rx = false
			 Uart_driver->start_rx = TRUE;			//inicio de trama ok
			 Uart_driver->stop_rx = FALSE;
			 Uart_driver->ptr_pool_rx = (mensaje_t *)QMPool_get_FROM_ISR(&Uart_driver->Pool_memoria, 0);//pedimos memoria
			 seed_crc = crc8_init();
			 //Uart_driver->ptr_pool_rx->msg[Uart_driver->ptr_pool_rx->lenght++] = c;
			 xTimerStartFromISR(Uart_driver->timerHndl5SecTimeout, &xHigherPriorityTaskWoken);
		 }
		 else{
			 if(c == ')' & Uart_driver->start_rx)
			 {
				 uint8_t crc_l;
				 uint8_t crc_h;
				 xTimerStopFromISR(Uart_driver->timerHndl5SecTimeout, &xHigherPriorityTaskWoken);
				 Uart_driver->start_rx = FALSE;				//mensaje recibido ok
				 Uart_driver->stop_rx = TRUE;
				 crc_sep(seed_crc, &crc_h, &crc_l);
				 Uart_driver->ptr_pool_rx->msg[Uart_driver->rxLen++] = crc_h;	//le ponemos el crc
				 Uart_driver->ptr_pool_rx->msg[Uart_driver->rxLen++] = crc_l;	//le ponemos el crc
				 Uart_driver->ptr_pool_rx->msg[Uart_driver->rxLen++] = '\0';
			 }
			 else{
				 if(Uart_driver->start_rx){
					 seed_crc = crc8_calc(seed_crc, (char *)&c, 1);	//calculo el crc a medida que llegan bytes
					 Uart_driver->ptr_pool_rx->msg[Uart_driver->rxLen++] = c;
					 xTimerResetFromISR(Uart_driver->timerHndl5SecTimeout, &xHigherPriorityTaskWoken);
				 }
			 }
		 }

		 if(Uart_driver->stop_rx){
			 Uart_driver->rxCounter = 0;
			 Uart_driver->stop_rx = FALSE;
			 Uart_driver->ptr_pool_rx->lenght = Uart_driver -> rxLen - 1; //chequear este -1
			 Uart_driver -> rxLen = 0;
			 xQueueSendFromISR(Uart_driver->onRxQueue, &Uart_driver->ptr_pool_rx, &xHigherPriorityTaskWoken);
		 }
	 }
	 else
	 {
		 Uart_driver->rxLen = LENGHT_MAX + 1; // revisar ese +1
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
		uartTxWrite(Uart_driver->uartValue, Uart_driver->ptr_pool_tx->msg[Uart_driver->txCounter]);
		Uart_driver -> txCounter ++;
	 }

	 if ( Uart_driver-> txCounter == Uart_driver->txLen)
	 {
		 uartTxWrite(Uart_driver->uartValue, '\r');
		 uartTxWrite(Uart_driver->uartValue, '\n');
		 uartCallbackClr(Uart_driver->uartValue, UART_TRANSMITER_FREE); // Deshabilito la interrupcion de Tx
		 //xTimerStartFromISR(Uart_driver->onTxTimeOut, &xTaskWokenByReceive); // Inicio el timer de timeout Tx
		 Uart_driver->ptr_pool_tx->lenght = 0;
		 Uart_driver->txLen = 0;
		 Uart_driver-> txCounter = 0;
		 //free memory
		 //Uart_driver->rxLen = 0;
		 QMPool_put_FROM_ISR( &Uart_driver->Pool_memoria, Uart_driver->ptr_pool_rx );
		 //QMPool_put_FROM_ISR( &Uart_driver->Pool_memoria, Uart_driver->ptr_pool_tx );
	 }

	 taskEXIT_CRITICAL_FROM_ISR( uxSavedInterrumptStatus );

	 // Evaluamos el pedido de cambio de contexto necesario
	 if ( xTaskWokenByReceive != pdFALSE )
	 {
		 taskYIELD();
	 }

 }

