/*
 * waterflow.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/waterflow.h>

#include <Hal/gpio.h>
#include <Hal/timer.h>

#define WATERFLOW_MEASURE_MS		1000

static GPIO_info_t WATERFLOW_io = {GPIOE, { GPIO_PIN_9, GPIO_MODE_IT_RISING, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH}};
static uint32_t WATERFLOW_pulse = 0;
static uint32_t WATERFLOW_cnt = false;

static uint32_t WATERFLOW_value = 0;

static void WATERFLOW_interrupt1ms(void);

void WATERFLOW_init(void){
	HAL_GPIO_Init(WATERFLOW_io.port, &WATERFLOW_io.init_info);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	TIMER_attach_intr_1ms(WATERFLOW_interrupt1ms);
}
/**
 * Get water flow (unit (L/m))
 */
uint32_t WATERFLOW_get(WATERFLOW_Id_t id){
	return WATERFLOW_value;
}

bool WATERFLOW_isError(WATERFLOW_Id_t id){
	return false;
}


void HAL_GPIO_EXTI_Callback(uint16_t pin){
	if(pin == WATERFLOW_io.init_info.Pin){
		WATERFLOW_pulse++;
	}
}


static void WATERFLOW_interrupt1ms(void){
	WATERFLOW_cnt++;
	if(WATERFLOW_cnt > WATERFLOW_MEASURE_MS){
		WATERFLOW_value = (uint32_t)((float)WATERFLOW_pulse * 7.5);
		WATERFLOW_cnt = 0;
		WATERFLOW_pulse = 0;
	}
}

