/*
 * convert.c
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#include "extra_func.h"


/*=====[Inclusions of private function dependencies]=========================*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "task.h"
#include "crc8.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/


char convert_Hex(uint8_t val){
	switch(val){
	case 0:
		val = '0';
		break;
	case 1:
		val = '1';
		break;
	case 2:
		val = '2';
		break;
	case 3:
		val = '3';
		break;
	case 4:
		val = '4';
		break;
	case 5:
		val = '5';
		break;
	case 6:
		val = '6';
		break;
	case 7:
		val = '7';
		break;
	case 8:
		val = '8';
		break;
	case 9:
		val = '9';
		break;
	case 10:
		val = 'A';
		break;
	case 11:
		val = 'B';
		break;
	case 12:
		val = 'C';
		break;
	case 13:
		val = 'D';
		break;
	case 14:
		val = 'E';
		break;
	case 15:
		val = 'F';
		break;
	default:

		break;
	}
	return val;

}

bool_t validate_String(mensaje_t * mens){
	bool_t result = FALSE;
	uint8_t i;
	if (mens->lenght - 2 > 0){					//corroboro que esto sea > 0 aunque no deberia darse lo opuesto
		taskENTER_CRITICAL();
		for (i = 0; i < mens->lenght - 2; i++) {//-2 para no contabilizar el crc
			if ((mens->msg[i] >= A && mens->msg[i] <= Z) || (mens->msg[i] >= a && mens->msg[i] <= z))//chequeo que este dentro de los parametros ascii requeridos de a-z y A-Z
			{
				result = TRUE;
			}
			else {
				result = FALSE;
				break;
			}
		}
		taskEXIT_CRITICAL();
	}
	return result;

}

bool_t check_Crc(mensaje_t * mens){
	bool_t result = FALSE;
	uint8_t crc_l;
	uint8_t crc_h;
	uint8_t crc_h_aux; //= convert_hex( mens->msg[mens->lenght-1]);
	uint8_t crc_l_aux; //= convert_hex( mens->msg[mens->lenght-0]);
	uint8_t i;
	volatile uint8_t seed_crc;

	seed_crc = crc8_init();
	taskENTER_CRITICAL();
	crc_h_aux = convert_Hex( mens->msg[mens->lenght-2]);
	crc_l_aux = convert_Hex( mens->msg[mens->lenght-1]);

	for(i = 0; i< mens->lenght - 2; i++ ){
		seed_crc = crc8_calc(seed_crc, (char *)&mens->msg[i], 1);
	}
	taskEXIT_CRITICAL();

	crc_sep(seed_crc, &crc_h, &crc_l); //a lenght le saco el crc para que calcule el crc del msj solo
	if(crc_l == crc_l_aux && crc_h == crc_h_aux){
		result = TRUE;;
	}

	return result;

}

bool_t lowercase_String(mensaje_t * const msg) {
	uint8_t i;
	bool_t result;
	taskENTER_CRITICAL();
	//xSemaphoreTake(mutex_analize, portMAX_DELAY);
	for (i = 1; i < msg->lenght - 2; i++) {
		if (msg->msg[i] >= A && msg->msg[i] <= Z)//chequeo que este dentro de los parametros ascii requeridos de a-z y A-Z
		{
			msg->msg[i] = msg->msg[i] + 32;

		}
		else {
			if (msg->msg[i] < a || msg->msg[i] > z){
				result = FALSE;
				break;
			}
		}
		result = TRUE;
	}
	//xSemaphoreGive(mutex_analize);
	taskEXIT_CRITICAL();
	return result;
}

bool_t uppercase_String(mensaje_t * const msg) {

	uint8_t i;
	bool_t result;
	taskENTER_CRITICAL();
	//xSemaphoreTake(mutex_analize, portMAX_DELAY);
	for (i = 1; i < msg->lenght - 2; i++) {
		if (msg->msg[i] >= a && msg->msg[i] <= z)//chequeo que este dentro de los parametros ascii requeridos de a-z y A-Z
		{
			msg->msg[i] = msg->msg[i] - 32;
		}
		else {
			if (msg->msg[i] < A || msg->msg[i] > Z){
				result = FALSE;
				break;
			}
		}
		result = TRUE;
	}
	//xSemaphoreGive(mutex_analize);
	taskEXIT_CRITICAL();
	return result;
}


void crc_sep(uint8_t crc, uint8_t *crc_h, uint8_t *crc_l ){

	uint8_t c_l;
	uint8_t c_h;

	c_h = crc & 0xF0;		//me quedo con parte alta
	c_h = c_h >> 4;
	c_h = convert_Hex(c_h);
	c_l = crc & 0x0F;		//me quedo con parte baja
	c_l = convert_Hex(c_l);
	*crc_l = c_l;
	*crc_h = c_h;
}


