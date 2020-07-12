/*
 * layer2.c
 *
 *  Created on: Jul 10, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_TP1_INC_LAYER2_C_
#define PROGRAM_RTOS2_TP1_INC_LAYER2_C_

#include "sapi.h"
#include "types.h"
#include "driver.h"

void layer2_constructor(layer2_t * me, uint8_t mode);//, char c0, char *dato0, char *crc0);
void layer2_destructor(layer2_t * me, uint8_t mode);
uint8_t layer2_frame_separation(layer2_t * me, mensaje_t * frame);
uint8_t layer2_check_crc(layer2_t * const me);
void layer2_msg_gen(layer2_t * me, driver_t * Uart_driver);

#endif /* PROGRAM_RTOS2_TP1_INC_LAYER2_C_ */
