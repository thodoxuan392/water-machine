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
#include <Device/solenoid.h>
#include <Device/led.h>


typedef enum {
	MACHINE_ID_1 = 0,
	MACHINE_ID_2,
	MACHINE_ID_3,
	MACHINE_ID_MAX
}MACHINE_Id_t;

typedef struct {
	RFID_Id_t rfid_id;
	PLACEDPOINT_Id_t placed_point_id;
	SOLENOID_Id solenoid_id;
	WATERFLOW_Id_t water_flow_id;
	LED_Id_t led_id;

	RFID_t rfid;
	uint8_t placed_point_status;
	uint8_t solenoid_status;
	uint8_t water_flow_status;
	uint8_t rfid_placed_status;
	uint8_t led_status;
	uint8_t error;
}MACHINE_t;


bool STATEMACHINE_init();
void STATEMACHINE_run();
bool STATEMACHINE_openVAN(MACHINE_Id_t id, uint16_t volume);
bool STATEMACHINE_cancelOpenVAN(MACHINE_Id_t id);
bool STATEMACHINE_playSound(MACHINE_Id_t id, uint32_t soundIndex);
uint8_t STATEMACHINE_updateRFID(MACHINE_Id_t id, RFID_t *rfid);
bool STATEMACHINE_controlIo(MACHINE_Id_t id, uint8_t ioMask);
MACHINE_t* STATEMACHINE_getMachine(MACHINE_Id_t id);

#endif /* INC_APP_STATEMACHINE_H_ */
