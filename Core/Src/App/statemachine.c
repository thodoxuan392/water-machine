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
#include "Lib/scheduler/scheduler.h"
#include "Lib/utils/utils_logger.h"

#include "Device/placedpoint.h"
#include "Device/rfid.h"
#include "Device/sound.h"
#include "Device/waterflow.h"

enum {
	SM_IDLE,
	SM_OPEN_VAN,
	SM_PLAY_SOUND,
	SM_UPDATE_RFID,
};

// Machine
static void MACHINE_update(MACHINE_Id_t id);
static void SM_idle();
static void SM_openVan();
static void SM_playSound();
static void SM_updateRfid();
// Utils
static void SM_timeout();
static void SM_printf();


static uint8_t prev_state = SM_IDLE;
static uint8_t state = SM_IDLE;
static const char * state_name[] = {
		[SM_IDLE] = "SM_IDLE\r\n",
		[SM_OPEN_VAN] = "SM_OPEN_VAN\r\n",
		[SM_PLAY_SOUND] = "SM_PLAY_SOUND\r\n",
		[SM_UPDATE_RFID] = "SM_UPDATE_RFID\r\n"
};
static uint8_t timeout_task_id;
static bool timeout = false;

static MACHINE_t machine[] = {
		[MACHINE_ID_1] = {
				.rfid_id = RFID_ID_1,
				.sound_id = SOUND_ID_1,
				.placed_point_id = PLACEDPOINT_ID_1,
				.water_flow_id = WATERFLOW_ID_1,
				.rfid = {{0}},
				.placed_point_status = 0,
				.rfid_placed_status = 0,
				.error = 0
		},
		[MACHINE_ID_2] = {
				.rfid_id = RFID_ID_2,
				.sound_id = SOUND_ID_2,
				.placed_point_id = PLACEDPOINT_ID_2,
				.water_flow_id = WATERFLOW_ID_2,
				.rfid = {{0}},
				.placed_point_status = 0,
				.rfid_placed_status = 0,
				.error = 0
		}
};
bool STATEMACHINE_init(){
}

void STATEMACHINE_run(){
	STATUSREPORTER_run();
	COMMANDHANDLER_run();
	SCH_Dispatch_Tasks();
	MACHINE_update(MACHINE_ID_1);
	MACHINE_update(MACHINE_ID_2);
	switch (state) {
		case SM_IDLE:
			SM_idle();
			break;
		case SM_OPEN_VAN:
			SM_openVan();
			break;
		case SM_PLAY_SOUND:
			SM_playSound();
			break;
		case SM_UPDATE_RFID:
			SM_updateRfid();
			break;
		default:
			break;
	}
	SM_printf();
	prev_state = state;
}

bool STATEMACHINE_openVAN(MACHINE_Id_t id, bool enable){

}

bool STATEMACHINE_playSound(MACHINE_Id_t id, uint32_t soundIndex){

}

bool STATEMACHINE_updateRFID(MACHINE_Id_t id, RFID_Id_t *rfid){

}

MACHINE_t* STATEMACHINE_getMachine(MACHINE_Id_t id){

}

static void MACHINE_update(MACHINE_Id_t id){
	RFID_get(machine[id].rfid_id, &machine[id].rfid);
	machine[id].placed_point_status = PLACEDPOINT_isPlaced(machine[id].placed_point_id);
	machine[id].water_flow_status = WATERFLOW_get(machine[id].water_flow_status);
	machine[id].rfid_placed_status = RFID_isPlaced(machine[id].rfid_placed_status);
	machine[id].error = (uint8_t)PLACEDPOINT_isError(machine[id].placed_point_id) |
						((uint8_t)WATERFLOW_isError(machine[id].water_flow_id) << 1) |
						((uint8_t)RFID_isError(machine[id].rfid_id) << 2) |
						((uint8_t)SOUND_isError(machine[id].placed_point_id) << 3);
}


static void SM_idle(){

}

static void SM_openVan(){

}

static void SM_playSound(){

}

static void SM_updateRfid(){

}

static void SM_timeout(){
	timeout = true;
}

static void SM_printf(){
	if(prev_state != state){
		utils_log_info(state_name[state]);
	}
}
