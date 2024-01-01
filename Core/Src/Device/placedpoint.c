/*
 * placedpoint.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/placedpoint.h>

#include <Hal/gpio.h>

static GPIO_info_t PLACEDPOINT_ioTable[] = {
	[PLACEDPOINT_ID_1] = {GPIOB, { GPIO_PIN_9, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}},
	[PLACEDPOINT_ID_2] = {GPIOE, { GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}},
	[PLACEDPOINT_ID_3] = {GPIOE, { GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}},
};

bool PLACEDPOINT_init(void){
	for (int id = 0; id < PLACEDPOINT_ID_MAX; ++id) {
		HAL_GPIO_Init(PLACEDPOINT_ioTable[id].port, &PLACEDPOINT_ioTable[id].init_info);
	}
}

bool PLACEDPOINT_isPlaced(PLACEDPOINT_Id_t id){
	return (HAL_GPIO_ReadPin(PLACEDPOINT_ioTable[id].port, PLACEDPOINT_ioTable[id].init_info.Pin) == GPIO_PIN_SET);
}


bool PLACEDPOINT_isError(PLACEDPOINT_Id_t id){
	return false;
}
