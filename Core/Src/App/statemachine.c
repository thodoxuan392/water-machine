/*
 * statemachine.c
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */


#include "config.h"
#include "App/statemachine.h"
#include "App/statusreporter.h"
#include "App/commandhandler.h"
#include "App/protocol.h"

#include "Lib/scheduler/scheduler.h"
#include "Lib/utils/utils_logger.h"

#include "DeviceManager/vanmanager.h"

#include "Device/placedpoint.h"
#include "Device/rfid.h"
#include "Device/sound.h"
#include "Device/waterflow.h"
#include "Device/solenoid.h"

// Machine
static void MACHINE_update(MACHINE_Id_t id);
static void STATEMACHINE_onOpenVanCompletedCallback(uint8_t solenoidId,  uint8_t success);

static MACHINE_t machine[] = {
		[MACHINE_ID_1] = {
				.rfid_id = RFID_ID_1,
				.placed_point_id = PLACEDPOINT_ID_1,
				.solenoid_id = SOLENOID_ID_1,
				.water_flow_id = WATERFLOW_ID_1,
				.rfid = {{0}},
				.placed_point_status = 0,
				.rfid_placed_status = 0,
				.error = 0
		},
		[MACHINE_ID_2] = {
				.rfid_id = RFID_ID_2,
				.placed_point_id = PLACEDPOINT_ID_2,
				.solenoid_id = SOLENOID_ID_2,
				.water_flow_id = WATERFLOW_ID_2,
				.rfid = {{0}},
				.placed_point_status = 0,
				.rfid_placed_status = 0,
				.error = 0
		},
		[MACHINE_ID_3] = {
				.rfid_id = RFID_ID_3,
				.placed_point_id = PLACEDPOINT_ID_3,
				.solenoid_id = SOLENOID_ID_3,
				.water_flow_id = WATERFLOW_ID_3,
				.rfid = {{0}},
				.placed_point_status = 0,
				.rfid_placed_status = 0,
				.error = 0
		}
};

bool STATEMACHINE_init(){
	VANMANAGER_setOnOpenVanCompletedCallback(STATEMACHINE_onOpenVanCompletedCallback);
}

void STATEMACHINE_run(){
	// Update machine status
	for (int id = 0; id < MACHINE_ID_MAX; ++id) {
		MACHINE_update(id);
	}
	// Check RFID detected
	for (int id = 0; id < MACHINE_ID_MAX; ++id) {
		if(RFID_isDetected(machine[id].rfid_id)){
			RFID_clearDetected(machine[id].rfid_id);
			STATUSREPORTER_reportRfidDetected(id);
		}
	}
}

bool STATEMACHINE_openVAN(MACHINE_Id_t id, uint16_t volume){
	return VANMANAGER_openVan(id, volume);
}

bool STATEMACHINE_cancelOpenVAN(MACHINE_Id_t id){
	return VANMANAGER_cancelOpenVan(id);
}

bool STATEMACHINE_playSound(MACHINE_Id_t id, uint32_t soundIndex){
	return SOUND_play(soundIndex);
}

uint8_t STATEMACHINE_updateRFID(MACHINE_Id_t id, RFID_t *rfid){
	return RFID_set(machine[id].rfid_id, rfid);
}

MACHINE_t* STATEMACHINE_getMachine(MACHINE_Id_t id){
	return &machine[id];
}

static void MACHINE_update(MACHINE_Id_t id){
	RFID_get(machine[id].rfid_id, &machine[id].rfid);
	machine[id].placed_point_status = PLACEDPOINT_isPlaced(machine[id].placed_point_id);
	machine[id].solenoid_status = SOLENOIS_isEnable(machine[id].solenoid_id);
	machine[id].water_flow_status = WATERFLOW_getIn2CcPerSecond(machine[id].water_flow_status);
	machine[id].rfid_placed_status = RFID_isPlaced(machine[id].rfid_id);
	machine[id].error = (uint8_t)PLACEDPOINT_isError(machine[id].placed_point_id) |
						((uint8_t)WATERFLOW_isError(machine[id].water_flow_id) << 1) |
						((uint8_t)RFID_isError(machine[id].rfid_id) << 2) |
						((uint8_t)SOUND_isError() << 3);
}

static void STATEMACHINE_onOpenVanCompletedCallback(uint8_t solenoidId, uint8_t success){
	uint8_t result = success? RESULT_SUCCESS : RESULT_FAILED;
	uint8_t machineId;
	for (int id = 0; id < MACHINE_ID_MAX; ++id) {
		if(machine[id].solenoid_id == solenoidId){
			machineId = id;
			break;
		}
	}
	COMMANDHANDLER_sendCommandOpenVanResult(machineId, result);
}

