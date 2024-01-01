/*
 * solenoid.h
 *
 *  Created on: Dec 30, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_SOLENOID_H_
#define INC_DEVICE_SOLENOID_H_

#include <stdbool.h>

typedef enum {
	SOLENOID_ID_1 = 0,
	SOLENOID_ID_2,
	SOLENOID_ID_3,
	SOLENOID_ID_MAX
}SOLENOID_Id;

void SOLENOID_init(void);
void SOLENOID_set(SOLENOID_Id id, bool enable);
bool SOLENOIS_isEnable(SOLENOID_Id id);


#endif /* INC_DEVICE_SOLENOID_H_ */
