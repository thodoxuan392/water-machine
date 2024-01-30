/*
 * waterflow.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/waterflow.h>

#include <Hal/gpio.h>
#include <Hal/timer.h>


typedef struct {
	uint32_t counter;
	uint32_t pulse;
	uint32_t value;
	bool error;
}WATERFLOW_Handle;

static GPIO_info_t WATERFLOW_ioTable[] = {
	[WATERFLOW_ID_1] = {GPIOE, { GPIO_PIN_3, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
	[WATERFLOW_ID_2] = {GPIOE, { GPIO_PIN_4, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
	[WATERFLOW_ID_3] = {GPIOE, { GPIO_PIN_5, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
};

static WATERFLOW_Handle WATERFLOW_handleTable[] = {
	[WATERFLOW_ID_1]  = {
		.counter = 0,
		.pulse = 0,
		.value = 0,
		.error = false
	},
	[WATERFLOW_ID_2]  = {
		.counter = 0,
		.pulse = 0,
		.value = 0,
		.error = false
	},
	[WATERFLOW_ID_3]  = {
		.counter = 0,
		.pulse = 0,
		.value = 0,
		.error = false
	},
};

static void WATERFLOW_interrupt1ms(void);

void WATERFLOW_init(void){
	TIMER_attach_intr_1ms(WATERFLOW_interrupt1ms);
	for (int id = 0; id < WATERFLOW_ID_MAX; ++id) {
		HAL_GPIO_Init(WATERFLOW_ioTable[id].port, &WATERFLOW_ioTable[id].init_info);
	}
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
/**
 * Get water flow (unit (L/m))
 */
uint32_t WATERFLOW_get(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].value;
}

uint32_t WATERFLOW_getIn2CcPerSecond(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].value * 100 / 3;
}

bool WATERFLOW_isError(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].error;
}

void HAL_GPIO_EXTI_Callback(uint16_t pin){
	for (int id = 0; id < WATERFLOW_ID_MAX; ++id) {
		if(WATERFLOW_ioTable[id].init_info.Pin == pin){
			WATERFLOW_handleTable[id].pulse++;
		}
	}
}


void WATERFLOW_test(void){
	if(HAL_GPIO_ReadPin(WATERFLOW_ioTable[WATERFLOW_ID_2].port, WATERFLOW_ioTable[WATERFLOW_ID_2].init_info.Pin) == GPIO_PIN_RESET){
		uint8_t a = 1;
	}
}

static void WATERFLOW_interrupt1ms(void){
	for (int id = 0; id < WATERFLOW_ID_MAX; ++id) {
		WATERFLOW_handleTable[id].counter++;
		if(WATERFLOW_handleTable[id].counter > 1000){
			WATERFLOW_handleTable[id].value = (uint32_t)((float)WATERFLOW_handleTable[id].pulse / 7.5);
			WATERFLOW_handleTable[id].counter = 0;
			WATERFLOW_handleTable[id].pulse = 0;
		}
	}

}

