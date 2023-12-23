/*
 * placedpoint.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_PLACEDPOINT_H_
#define INC_DEVICE_PLACEDPOINT_H_

#include <stdbool.h>

typedef enum {
	PLACEDPOINT_ID_1 = 0,
	PLACEDPOINT_ID_2
}PLACEDPOINT_Id_t;

bool PLACEDPOINT_init(void);
bool PLACEDPOINT_isPlaced(PLACEDPOINT_Id_t id);
bool PLACEDPOINT_isError(PLACEDPOINT_Id_t id);

#endif /* INC_DEVICE_PLACEDPOINT_H_ */
