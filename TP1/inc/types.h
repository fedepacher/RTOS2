/*
 * types.h
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_EJ_1_PARTE_1_INC_TYPES_H_
#define PROGRAM_RTOS2_EJ_1_PARTE_1_INC_TYPES_H_

/*const char M = 'M';
const char m = 'm';
const char a = 'a';
const char A = 'A';
const char z = 'z';
const char Z = 'Z';*/

#define M  'M'
#define m  'm'
#define a  'a'
#define A  'A'
#define z  'z'
#define Z  'Z'

#define PACKET_SIZE  50
#define POOL_TOTAL_BLOCKS 5
#define POOL_SIZE POOL_TOTAL_BLOCKS*PACKET_SIZE //Tamanio total del pool

static gpioMap_t teclas [] = {TEC1, TEC2};
#define N_TECLAS  sizeof(teclas) / sizeof(gpioMap_t)

typedef struct{
	char *msg;
	uint8_t lenght;
}mensaje_t;


#endif /* PROGRAM_RTOS2_EJ_1_PARTE_1_INC_TYPES_H_ */
