/*
 * led.c
 *
 *  Created on: Feb 4, 2024
 *      Author: thodo
 */


#include <Device/led.h>

#include <Hal/gpio.h>


static GPIO_info_t LED_ioTable[] = {
	[LED_ID_1] = {GPIOE, { GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW}},
	[LED_ID_2] = {GPIOE, { GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW}},
	[LED_ID_3] = {GPIOE, { GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW}}
};

void LED_init(void){
	for (int led_id = 0; led_id < LED_ID_MAX; ++led_id) {
		HAL_GPIO_Init(LED_ioTable[led_id].port, &LED_ioTable[led_id].init_info);
		HAL_GPIO_WritePin(LED_ioTable[led_id].port, LED_ioTable[led_id].init_info.Pin, GPIO_PIN_RESET);
	}
}

void LED_set(LED_Id_t id, bool enable){
	HAL_GPIO_WritePin(LED_ioTable[id].port, LED_ioTable[id].init_info.Pin, enable);
}

bool LED_isEnable(LED_Id_t id){
	return HAL_GPIO_ReadPin(LED_ioTable[id].port, LED_ioTable[id].init_info.Pin) == GPIO_PIN_SET;
}
