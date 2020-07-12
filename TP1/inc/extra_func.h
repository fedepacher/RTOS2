/*
 * convert.h
 *
 *  Created on: Jul 7, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_TP1_INC_EXTRA_FUNC_H_
#define PROGRAM_RTOS2_TP1_INC_EXTRA_FUNC_H_

#include "sapi.h"
#include "types.h"
#include "crc8.h"

/**
 * @brief  Convert to hexadecimal.
 * @param  val	value to be converted.
 * @retval hexadecimal caracter.
 */
char convert_Hex(uint8_t val);

/**
 * @brief  Check if received frame was ok
 * @param  mens		frame to be checked.
 * @retval 1 if ok.
 */
uint8_t validate_String(layer2_t * msg);

/**
 * @brief  Check if received CRC frame was ok
 * @param  mens		frame to be checked.
 * @retval true if ok.
 */
uint8_t check_Crc(layer2_t * msg);

/**
 * @brief  Convert to lowercase
 * @param  msg		frame to be checked.
 * @retval true if ok.
 */
uint8_t lowercase_String(mensaje_t * msg);

/**
 * @brief  Convert to uppercase
 * @param  msg		frame to be checked.
 * @retval true if ok.
 */
uint8_t uppercase_String(mensaje_t * msg);

/**
 * @brief  Separate 1 byte CRC into 2 byte CRC -> CRC_H(4b) CRC_L(4b) -> CRC_H(8b) CRC_L(8b).
 * @param  crc		value to be separated.
 * @param  crc_h	value were 8bytes crc high will be stored
 * @param  crc_l	value were 8bytes crc low will be stored
 * @retval none.
 */
void crc_sep(uint8_t crc, uint8_t *crc_h, uint8_t *crc_l );

/**
 * @brief  Calculate CRC.
 * @param  msg		msg to calculate crc.
 * @param  crc		initial seed
 * @retval crc value.
 */
uint8_t crc_calculation(layer2_t * msg, uint8_t crc);

#endif /* PROGRAM_RTOS2_TP1_INC_EXTRA_FUNC_H_ */
