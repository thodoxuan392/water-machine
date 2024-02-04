/*
 * waterflow.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/waterflow.h>

#include <Hal/gpio.h>
#include <Hal/timer.h>

#define WATERFLOW_VOLUME_PULSE_MAPPING_MAX_LEN	100
#define WATERFLOW_VOLUME_PULSE_SCALE_DEFAULT 	(500/288)

typedef struct {
	uint32_t volume;
	uint32_t pulse;
}WATERFLOW_VolumePulseMapping;

typedef struct {
	uint32_t counter;
	uint32_t pulseV1;
	uint32_t pulseV2;
	uint32_t value;
	bool error;
	WATERFLOW_VolumePulseMapping mapping[WATERFLOW_VOLUME_PULSE_MAPPING_MAX_LEN];
}WATERFLOW_Handle;

static GPIO_info_t WATERFLOW_ioTable[] = {
	[WATERFLOW_ID_1] = {GPIOE, { GPIO_PIN_3, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
	[WATERFLOW_ID_2] = {GPIOE, { GPIO_PIN_4, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
	[WATERFLOW_ID_3] = {GPIOE, { GPIO_PIN_5, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
};

static WATERFLOW_Handle WATERFLOW_handleTable[] = {
	[WATERFLOW_ID_1]  = {
		.counter = 0,
		.pulseV1 = 0,
		.pulseV2 = 0,
		.value = 0,
		.error = false,
		.mapping = {
				{ 500, 228 },
				{ 1000, 455 },
				{ 1500, 683 },
				{ 2000, 910 },
				{ 2500, 1138 },
				{ 3000, 1365 },
				{ 3500, 1593 },
				{ 4000, 1820 },
				{ 4500, 2048 },
				{ 5000, 2275 },
				{ 5500, 2503 },
				{ 6000, 2730 },
				{ 6500, 2958 },
				{ 7000, 3185 },
				{ 7500, 3413 },
				{ 8000, 3640 },
				{ 8500, 3868 },
				{ 9000, 4095 },
				{ 9500, 4323 },
				{ 10000, 4550 },
				{ 10500, 4778 },
				{ 11000, 5005 },
				{ 11500, 5233 },
				{ 12000, 5460 },
				{ 12500, 5688 },
				{ 13000, 5915 },
				{ 13500, 6143 },
				{ 14000, 6370 },
				{ 14500, 6598 },
				{ 15000, 6825 },
				{ 15500, 7053 },
				{ 16000, 7280 },
				{ 16500, 7508 },
				{ 17000, 7735 },
				{ 17500, 7963 },
				{ 18000, 8190 },
				{ 18500, 8418 },
				{ 19000, 8645 },
				{ 19500, 8873 },
				{ 20000, 9100 },
				{ 20500, 9328 },
				{ 21000, 9555 },
		}
	},
	[WATERFLOW_ID_2]  = {
		.counter = 0,
		.pulseV1 = 0,
		.pulseV2 = 0,
		.value = 0,
		.error = false,
		.mapping = {
				{ 500, 228 },
				{ 1000, 455 },
				{ 1500, 683 },
				{ 2000, 910 },
				{ 2500, 1138 },
				{ 3000, 1365 },
				{ 3500, 1593 },
				{ 4000, 1820 },
				{ 4500, 2048 },
				{ 5000, 2275 },
				{ 5500, 2503 },
				{ 6000, 2730 },
				{ 6500, 2958 },
				{ 7000, 3185 },
				{ 7500, 3413 },
				{ 8000, 3640 },
				{ 8500, 3868 },
				{ 9000, 4095 },
				{ 9500, 4323 },
				{ 10000, 4550 },
				{ 10500, 4778 },
				{ 11000, 5005 },
				{ 11500, 5233 },
				{ 12000, 5460 },
				{ 12500, 5688 },
				{ 13000, 5915 },
				{ 13500, 6143 },
				{ 14000, 6370 },
				{ 14500, 6598 },
				{ 15000, 6825 },
				{ 15500, 7053 },
				{ 16000, 7280 },
				{ 16500, 7508 },
				{ 17000, 7735 },
				{ 17500, 7963 },
				{ 18000, 8190 },
				{ 18500, 8418 },
				{ 19000, 8645 },
				{ 19500, 8873 },
				{ 20000, 9100 },
				{ 20500, 9328 },
				{ 21000, 9555 },
		}
	},
	[WATERFLOW_ID_3]  = {
		.counter = 0,
		.pulseV1 = 0,
		.pulseV2 = 0,
		.value = 0,
		.error = false,
		.mapping = {
				{ 500, 228 },
				{ 1000, 455 },
				{ 1500, 683 },
				{ 2000, 910 },
				{ 2500, 1138 },
				{ 3000, 1365 },
				{ 3500, 1593 },
				{ 4000, 1820 },
				{ 4500, 2048 },
				{ 5000, 2275 },
				{ 5500, 2503 },
				{ 6000, 2730 },
				{ 6500, 2958 },
				{ 7000, 3185 },
				{ 7500, 3413 },
				{ 8000, 3640 },
				{ 8500, 3868 },
				{ 9000, 4095 },
				{ 9500, 4323 },
				{ 10000, 4550 },
				{ 10500, 4778 },
				{ 11000, 5005 },
				{ 11500, 5233 },
				{ 12000, 5460 },
				{ 12500, 5688 },
				{ 13000, 5915 },
				{ 13500, 6143 },
				{ 14000, 6370 },
				{ 14500, 6598 },
				{ 15000, 6825 },
				{ 15500, 7053 },
				{ 16000, 7280 },
				{ 16500, 7508 },
				{ 17000, 7735 },
				{ 17500, 7963 },
				{ 18000, 8190 },
				{ 18500, 8418 },
				{ 19000, 8645 },
				{ 19500, 8873 },
				{ 20000, 9100 },
				{ 20500, 9328 },
				{ 21000, 9555 },
		}
	},
};

static void WATERFLOW_interrupt1ms(void);
static float WATERFLOW_getCalibrateByPulse(uint32_t pulse);

void WATERFLOW_init(void){
	for (int id = 0; id < WATERFLOW_ID_MAX; ++id) {
		HAL_GPIO_Init(WATERFLOW_ioTable[id].port, &WATERFLOW_ioTable[id].init_info);
	}
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	TIMER_attach_intr_1ms(WATERFLOW_interrupt1ms);
}

uint32_t WATERFLOW_getIn2CcPerSecond(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].value * 100 / 3;
}

#if defined(WATERFLOW_SW_V1)
uint32_t WATERFLOW_get(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].value;
}
#elif defined(WATERFLOW_SW_V2)
uint32_t WATERFLOW_getPulse(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].pulseV2;
}

uint32_t WATERFLOW_getPulseByVolume(WATERFLOW_Id_t id, uint32_t volume){

	for (int var = 0; var < WATERFLOW_VOLUME_PULSE_MAPPING_MAX_LEN - 1; ++var) {
		if(volume >= WATERFLOW_handleTable[id].mapping[var].volume && 
			volume <= WATERFLOW_handleTable[id].mapping[var + 1].volume){
			if(volume == WATERFLOW_handleTable[id].mapping[var].volume){
				return WATERFLOW_handleTable[id].mapping[var].pulse;
			}else if(volume == WATERFLOW_handleTable[id].mapping[var + 1].volume){
				return WATERFLOW_handleTable[id].mapping[var + 1].pulse;
			}
			break;
		}
	}
	return volume / WATERFLOW_VOLUME_PULSE_SCALE_DEFAULT;
}

void WATERFLOW_resetPulse(WATERFLOW_Id_t id){
	WATERFLOW_handleTable[id].pulseV2 = 0;
}
#endif

bool WATERFLOW_isError(WATERFLOW_Id_t id){
	return WATERFLOW_handleTable[id].error;
}

void HAL_GPIO_EXTI_Callback(uint16_t pin){
	for (int id = 0; id < WATERFLOW_ID_MAX; ++id) {
		if(WATERFLOW_ioTable[id].init_info.Pin == pin){
			WATERFLOW_handleTable[id].pulseV1++;
			WATERFLOW_handleTable[id].pulseV2++;
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
			float calib = WATERFLOW_getCalibrateByPulse(WATERFLOW_handleTable[id].pulseV1);
			WATERFLOW_handleTable[id].value = (uint32_t)((float)WATERFLOW_handleTable[id].pulseV1 * calib / 7.5);
			WATERFLOW_handleTable[id].counter = 0;
			WATERFLOW_handleTable[id].pulseV1 = 0;
		}
	}

}

static float WATERFLOW_getCalibrateByPulse(uint32_t pulse){
	if(pulse < 16){
		return 1.43f;
	}
	else if(pulse < 32.5){
		return 1.38f;
	}
	else if(pulse < 49.3){
		return 1.33f;
	}
	else if(pulse < 65.6){
		return 1.28f;
	}
	else if(pulse < 82){
		return 1.26f;
	}
	else {
		return 1.22f;
	}
}

