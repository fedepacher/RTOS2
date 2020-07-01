/*
 * userTasks.h
 *
 *  Created on: Jun 30, 2020
 *      Author: fedepacher
 */

#ifndef PROGRAM_RTOS2_EJ_1_PARTE_1_INC_USERTASKS_H_
#define PROGRAM_RTOS2_EJ_1_PARTE_1_INC_USERTASKS_H_



void init_tasks();
void tarea_tecla( void* taskParmPtr );
void tarea_a( void* taskParmPtr );
void tarea_b( void* taskParmPtr );
void tarea_c( void* taskParmPtr );

#endif /* PROGRAM_RTOS2_EJ_1_PARTE_1_INC_USERTASKS_H_ */
