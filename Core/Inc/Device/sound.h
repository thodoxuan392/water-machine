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
	SOUND_FILE_WELCOME = 0,
	SOUND_FILE_PLEASE_TAKE_A_BOTTLE,
	SOUND_FILE_THANK_YOU
}SOUND_File;

typedef enum {
	SOUND_ID_1 = 0,
	SOUND_ID_2,
	SOUND_ID_3,
	SOUND_ID_MAX
}SOUND_Id;

bool SOUND_init(void);
bool SOUND_play(SOUND_Id id, SOUND_File file);
bool SOUND_isError(SOUND_Id id);

#endif /* INC_DEVICE_SOUND_H_ */
