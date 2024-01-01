/*
 * solenoid.c
 *
 *  Created on: Dec 30, 2023
 *      Author: xuanthodo
 */

#ifndef SRC_DEVICE_SOLENOID_C_
#define SRC_DEVICE_SOLENOID_C_

#include <Device/solenoid.h>

#include <Hal/gpio.h>

static GPIO_info_t SOLENOID_ioTable[] = {
	[SOLENOID_ID_1] = {GPIOE, { GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW}},
	[SOLENOID_ID_2] = {GPIOE, { GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW}},
	[SOLENOID_ID_3] = {GPIOE, { GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW}}
};

void SOLENOID_init(void){
	for (int id = 0; id < SOLENOID_ID_MAX; ++id) {
		HAL_GPIO_Init(SOLENOID_ioTable[id].port, &SOLENOID_ioTable[id].init_info);
	}
}

void SOLENOID_set(SOLENOID_Id id, bool enable){
	HAL_GPIO_WritePin(SOLENOID_ioTable[id].port, SOLENOID_ioTable[id].init_info.Pin, enable);
}

bool SOLENOIS_isEnable(SOLENOID_Id id){
	return (HAL_GPIO_ReadPin(SOLENOID_ioTable[id].port, SOLENOID_ioTable[id].init_info.Pin) == GPIO_PIN_SET);
}


#endif /* SRC_DEVICE_SOLENOID_C_ */
