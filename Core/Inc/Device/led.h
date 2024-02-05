/*
 * led.h
 *
 *  Created on: Feb 4, 2024
 *      Author: thodo
 */

#ifndef INC_DEVICE_LED_H_
#define INC_DEVICE_LED_H_

#include <stdbool.h>

typedef enum {
	LED_ID_1 = 0,
	LED_ID_2,
	LED_ID_3,
	LED_ID_MAX
}LED_Id_t;

void LED_init(void);
void LED_set(LED_Id_t id, bool enable);
bool LED_isEnable(LED_Id_t id);


#endif /* INC_DEVICE_LED_H_ */
