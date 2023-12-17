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

typedef enum {
	WATERFLOW_ID_1 = 0,
	WATERFLOW_ID_2
}WATERFLOW_Id_t;

bool WATERFLOW_init(void);
bool WATERFLOW_run(WATERFLOW_Id_t id);

/**
 * Get water flow (unit (L/m))
 */
uint32_t WATERFLOW_get(WATERFLOW_Id_t id);

#endif /* INC_DEVICE_WATERFLOW_H_ */
