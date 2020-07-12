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
#include "string.h"
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

uint8_t validate_String(layer2_t * msg){

	uint8_t result = 0;
	uint8_t i;
	volatile uint8_t lenght;

	if (msg->dato != NULL){					//corroboro que esto sea > 0 aunque no deberia darse lo opuesto
		lenght = strlen((char *)msg->dato);
		for (i = 0; i < lenght; i++) {
			if ((msg->dato[i] >= A && msg->dato[i] <= Z) || (msg->dato[i] >= a && msg->dato[i] <= z))//chequeo que este dentro de los parametros ascii requeridos de a-z y A-Z
			{
				result = 1;
			}
			else {
				result = 0;
				break;
			}
		}
	}

	return result;

}

uint8_t check_Crc(layer2_t * msg){

	uint8_t crc_l;
	uint8_t crc_h;
	uint8_t crc_h_aux; //= convert_hex( mens->msg[mens->lenght-1]);
	uint8_t crc_l_aux; //= convert_hex( mens->msg[mens->lenght-0]);
	uint8_t i;
	volatile uint8_t seed_crc;
	volatile uint8_t lenght = strlen((char *)msg->dato);

	seed_crc = crc8_init();

	crc_h_aux = convert_Hex( msg->crc[0]);
	crc_l_aux = convert_Hex( msg->crc[1]);

	/*seed_crc = crc8_calc(seed_crc, &msg->c, 1);


	for(i = 0; i < lenght; i++ ){
		seed_crc = crc8_calc(seed_crc, (char *)&msg->dato[i], 1);
	}*/

	seed_crc = crc_calculation(msg, seed_crc);

	crc_sep(seed_crc, &crc_h, &crc_l); //a lenght le saco el crc para que calcule el crc del msj solo
	if(crc_l == crc_l_aux && crc_h == crc_h_aux){
		return 1;
	}

	return 0;

}


uint8_t crc_calculation(layer2_t * msg, uint8_t crc){
	volatile uint8_t seed;
	uint8_t i;
	volatile uint8_t lenght = strlen((char *)msg->dato);

	seed = crc8_calc(crc, &msg->c, 1);

	for(i = 0; i < lenght; i++ ){
		seed = crc8_calc(seed, (char *)&msg->dato[i], 1);
	}
	return seed;
}

uint8_t lowercase_String(mensaje_t * msg) {
	uint8_t i;
	uint8_t result = 0;
	volatile uint8_t lenght = strlen((char *)msg);

	for (i = 0; i < lenght; i++) {
		if (msg[i] >= A && msg[i] <= Z)//chequeo que este dentro de los parametros ascii requeridos de a-z y A-Z
		{
			msg[i] = msg[i] + 32;

		}
		else {
			if (msg[i] < a || msg[i] > z){
				result = 0;
				break;
			}
		}
		result = 1;
	}

	return result;
}

uint8_t uppercase_String(mensaje_t * msg) {

	uint8_t i;
	uint8_t result = 0;
	volatile uint8_t lenght = strlen((char *)msg);

	for (i = 0; i < lenght; i++) {
		if (msg[i] >= a && msg[i] <= z)//chequeo que este dentro de los parametros ascii requeridos de a-z y A-Z
		{
			msg[i] = msg[i] - 32;
		}
		else {
			if (msg[i] < A || msg[i] > Z){
				result = 0;
				break;
			}
		}
		result = 1;
	}

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


