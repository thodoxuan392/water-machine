/*
 * sound.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_SOUND_H_
#define INC_DEVICE_SOUND_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	SOUND_ID_1 = 0,
	SOUND_ID_2,
}SOUND_Id_t;

bool SOUND_init(void);
bool SOUND_play(SOUND_Id_t id, uint32_t index);
bool SOUND_stop(SOUND_Id_t id);
bool SOUND_pause(SOUND_Id_t id);
bool SOUND_resume(SOUND_Id_t id);

#endif /* INC_DEVICE_SOUND_H_ */
