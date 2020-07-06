/*
 * perifericos.h
 *
 *  Created on: Jun 30, 2020
 *      Author: fedepacher
 */
#ifndef PROGRAM_RTOS2_EJ_1_PARTE_1_INC_PERIFERICOS_H_
#define PROGRAM_RTOS2_EJ_1_PARTE_1_INC_PERIFERICOS_H_

#include "FreeRTOS.h"
#include "sapi.h"
//include "semphr.h"


#define PACKET_SIZE  50
#define POOL_TOTAL_BLOCKS 5
#define POOL_SIZE POOL_TOTAL_BLOCKS*PACKET_SIZE //Tamanio total del pool

typedef struct{
	char msj[20];
}mensaje_t;

static gpioMap_t teclas [] = {TEC1, TEC2};
#define N_TECLAS  sizeof(teclas) / sizeof(gpioMap_t)

/*
typedef struct
{
	gpioMap_t periferico;			//config
	mensaje_t msj;
	SemaphoreHandle_t mutex;
}periferico_t;*/



#endif /* PROGRAM_RTOS2_EJ_1_PARTE_1_INC_PERIFERICOS_H_ */
