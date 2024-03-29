/*
 * waterflow.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_WATERFLOW_H_
#define INC_DEVICE_WATERFLOW_H_

#include <stdbool.h>
#include <stdint.h>

//#define WATERFLOW_SW_V1
#define WATERFLOW_SW_V2
#define WATERFLOW_CALIBRATE	1.33f

typedef enum {
	WATERFLOW_ID_1 = 0,
	WATERFLOW_ID_2,
	WATERFLOW_ID_3,
	WATERFLOW_ID_MAX
}WATERFLOW_Id_t;

void WATERFLOW_init(void);
uint32_t WATERFLOW_getIn2CcPerSecond(WATERFLOW_Id_t id);
#if defined(WATERFLOW_SW_V1)
uint32_t WATERFLOW_get(WATERFLOW_Id_t id);
#elif defined(WATERFLOW_SW_V2)
uint32_t WATERFLOW_getPulse(WATERFLOW_Id_t id);
uint32_t WATERFLOW_getPulseByVolume(WATERFLOW_Id_t id, uint32_t volume);
void WATERFLOW_resetPulse(WATERFLOW_Id_t id);
#endif
bool WATERFLOW_isError(WATERFLOW_Id_t id);

#endif /* INC_DEVICE_WATERFLOW_H_ */
