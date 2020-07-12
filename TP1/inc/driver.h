/*=============================================================================
 * Copyright (c) 2019, Gonzalo Gontad <gonzalogontad@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/09/29
 * Version: v1.0
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef DRIVER_H_
#define DRIVER_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "string.h"
#include "sapi.h"
#include "timers.h"
#include "semphr.h"
#include "types.h"
#include "qmpool.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define PROTOCOL_TIMEOUT pdMS_TO_TICKS(3)
#define LENGHT_MAX 100

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/


typedef struct
{
    uartMap_t uartValue; //Numero de UART a utilizar
    uint32_t baudRate; //velocidad de transmision

    uint8_t txCounter; //contador de bytes transmitidos
    uint8_t rxCounter; //contador de bytes recibidos
    QueueHandle_t onTxQueue; //cola de transmision (por esta cola llegan los bloques de memoria a transmitir)
    QueueHandle_t onRxQueue; //cola de recepcion (por esta cola llegan los bloques de memoria recibidos)
    uint8_t txLen; //longitud del paquete en transmision
    uint8_t rxLen; //longitud del paquete recibido
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t start_timer;		//inicializa un timer para contabilizar que el si el dato no llego se reinicie contador de rx_uart
    xTimerHandle timerHndl5SecTimeout;
    bool_t start_rx;
    bool_t stop_rx;

    mensaje_t *dataRx;
    mensaje_t *dataTx;

    //layer2_t *message;

} driver_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

/**
 * @brief  Initialize uart.
 * @param  driver_t		uart to be configured
 * @retval true if initialization was ok, otherwise ret false.
 */
bool_t	driverInit	( driver_t* );

/**
 * @brief  Enable uart Tx interrupt.
 * @param  driver_t		enable uart int.
 * @retval none.
 */
void packetTX 	( driver_t* selectedUart);//,char *txBlock );

/**
 * @brief  Reset uart Rx when timer expired.
 * @param  pvParameters		uart param.
 * @retval none.
 */
void reset_uart_task( void* pvParameters );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif
