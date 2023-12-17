/*
 * timer.h
 *
 *  Created on: May 28, 2023
 *      Author: xuanthodo
 */

#ifndef INC_HAL_TIMER_H_
#define INC_HAL_TIMER_H_

#include "stdio.h"
#include "stdbool.h"

typedef void (*TIMER_fn)(void);

bool TIMER_init();
bool TIMER_attach_intr_1ms(void (*fn)(void));
void TIMER_test();

#endif /* INC_HAL_TIMER_H_ */
