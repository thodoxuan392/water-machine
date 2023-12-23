/*
 * statemachine.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_STATEMACHINE_H_
#define INC_APP_STATEMACHINE_H_

#include "stdio.h"
#include "stdbool.h"

#include <Device/rfid.h>
#include <Device/placedpoint.h>
#include <Device/sound.h>
#include <Device/waterflow.h>

typedef enum {
	MACHINE_ID_1 = 0,
	MACHINE_ID_2
}MACHINE_Id_t;

typedef struct {
	RFID_Id_t rfid_id;
	SOUND_Id_t sound_id;
	PLACEDPOINT_Id_t placed_point_id;
	WATERFLOW_Id_t water_flow_id;

	RFID_t rfid;
	uint8_t placed_point_status;
	uint8_t water_flow_status;
	uint8_t rfid_placed_status;
	uint8_t error;
}MACHINE_t;


bool STATEMACHINE_init();
void STATEMACHINE_run();
bool STATEMACHINE_openVAN(MACHINE_Id_t id, bool enable);
bool STATEMACHINE_playSound(MACHINE_Id_t id, uint32_t soundIndex);
bool STATEMACHINE_updateRFID(MACHINE_Id_t id, RFID_Id_t *rfid);
MACHINE_t* STATEMACHINE_getMachine(MACHINE_Id_t id);

#endif /* INC_APP_STATEMACHINE_H_ */
