/*
 * layer2.c
 *
 *  Created on: Jul 10, 2020
 *      Author: fedepacher
 */


#include "layer2.h"
#include "string.h"
#include "extra_func.h"


void layer2_constructor(layer2_t * me, uint8_t mode){
	//if(inHandlerMode())
	if (mode == HANDLER_MODE_ON){
		me->dato = (mensaje_t *)QMPool_get_FROM_ISR(memoryBlock_aux, 0);//pedimos memoria
	}
	else{
		me->dato = (mensaje_t *)QMPool_get(memoryBlock_aux, 0);//pedimos memoria
	}
	/*me.c = '\0';
	me.dato->msg = NULL;
	strncpy((char *)me.crc, '\0', 2);
*/
	//return me;
}

void layer2_destructor(layer2_t * me, uint8_t mode){

	if (mode == HANDLER_MODE_ON){
		QMPool_put_FROM_ISR( memoryBlock_aux, me );
	}
	else{
		QMPool_put( memoryBlock_aux, me );
	}
	me->c = '\0';
	me->dato = NULL;
	strncpy((char *)me->crc, '\0', 2);
}

uint8_t layer2_frame_separation(layer2_t *me, mensaje_t *frame){
	uint32_t i;
	//mensaje_t temp_dato = me->dato;	//guardo la posicion inicial de dato
	uint32_t lenght = strlen((char *)frame);

	if(me == NULL || frame == NULL){
		return 0;
	}

	//				1b	  nb	2b
	//				 |     |     |
	//trama			C	DATO	CRC
	me->c = frame[0];
	me->crc[0] = frame[lenght - 2];
	me->crc[1] = frame[lenght - 1];

	frame++;
	strncpy((char *)me->dato, (char *)frame, lenght - 3);

	lenght = strlen((char *)me->dato);
	me->dato[lenght + 1] = '\0';
	//agregar un '\0'

	return 1;
}



uint8_t layer2_check_crc(layer2_t * const me){
	if(check_Crc(me) == 1)
		return 1;
	return 0;
}


void layer2_msg_gen(layer2_t * me, driver_t * Uart_driver){

	uint32_t lenght =strlen((char *)me->dato);

	volatile uint8_t crc_seed = crc8_init();
	uint8_t crc_l;
	uint8_t crc_h;
	//mensaje_t msg;

	crc_seed = crc_calculation(me, crc_seed);
	crc_sep(crc_seed, &crc_h, &crc_l);

	Uart_driver->dataTx = (mensaje_t *)QMPool_get(memoryBlock, 0);//pedimos memoria

	//strncat((char *)msg, (char *)SOFRAME, 1);
	//strncat((char *)msg, (char *)me->c, 1);


	/**(Uart_driver->dataTx) = SOFRAME;
	Uart_driver->dataTx++;
	*(Uart_driver->dataTx) = me->c;
	Uart_driver->dataTx++;
	strncpy((char *)Uart_driver->dataTx, (char *)me->dato, lenght);
	Uart_driver->dataTx++;
	*(Uart_driver->dataTx) = crc_h;
	Uart_driver->dataTx++;
	*(Uart_driver->dataTx) = crc_l;
	Uart_driver->dataTx++;
	*(Uart_driver->dataTx) = EOFRAME;
	Uart_driver->dataTx++;
	*(Uart_driver->dataTx) = '\0';
*/
	Uart_driver->txLen = strlen((char*)Uart_driver->dataTx);
	packetTX(Uart_driver);

}
