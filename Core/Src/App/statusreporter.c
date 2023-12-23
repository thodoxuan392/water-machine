/*
 * statusreporter.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */

#include "config.h"
#include "App/statusreporter.h"
#include "App/protocol.h"
#include "Lib/scheduler/scheduler.h"

#define STATUSREPORT_INTERVAL		60 * 1000 	// 60s

static bool timeout_flag = true;

// Private function
static void STATUSREPORTER_report_status(MACHINE_Id_t id);
static void STATUSREPORTER_buildStatus(PROTOCOL_t * proto, MACHINE_Id_t machineId);
static void STATUSREPORTER_buildRifdDetected(PROTOCOL_t * proto, MACHINE_Id_t machineId);

static void STATUSREPORTER_timeout();

bool STATUSREPORTER_init(){

}

bool STATUSREPORTER_run(){
	if(timeout_flag){
		timeout_flag = false;
		// Publish status
		STATUSREPORTER_report_status(MACHINE_ID_1);
		STATUSREPORTER_report_status(MACHINE_ID_2);
		SCH_Add_Task(STATUSREPORTER_timeout, STATUSREPORT_INTERVAL, 0);
	}
}

void STATUSREPORTER_reportRfidDetected(MACHINE_Id_t id){
	PROTOCOL_t protocol;
	STATUSREPORTER_buildRifdDetected(&protocol, id);
	// Send message
	PROTOCOL_send(&protocol);
}

static void STATUSREPORTER_report_status(MACHINE_Id_t id){
	PROTOCOL_t protocol;
	STATUSREPORTER_buildStatus(&protocol, id);
	// Send message
	PROTOCOL_send(&protocol);
}
static void STATUSREPORTER_buildStatus(PROTOCOL_t * proto, MACHINE_Id_t machineId){
	MACHINE_t* machine = STATEMACHINE_getMachine(machineId);
	proto->protocol_id = PROTOCOL_ID_STATUS;
	proto->data_len = 0;
	proto->data[proto->data_len++] = machineId;
	proto->data[proto->data_len++] = machine->placed_point_status;
	proto->data[proto->data_len++] = machine->water_flow_status;
	proto->data[proto->data_len++] = machine->rfid_placed_status;
	proto->data[proto->data_len++] = machine->error;
}

static void STATUSREPORTER_buildRifdDetected(PROTOCOL_t * proto, MACHINE_Id_t machineId){
	MACHINE_t* machine = STATEMACHINE_getMachine(machineId);
	proto->protocol_id = PROTOCOL_ID_RFID_DETECTED;
	proto->data_len = 0;
	proto->data[proto->data_len++] = machineId;
	proto->data[proto->data_len++] = machine->rfid.id_len;
	for (int var = 0; var < machine->rfid.id_len; ++var) {
		proto->data[proto->data_len++] = machine->rfid.id[var];
	}
	proto->data[proto->data_len++] = (machine->rfid.money >> 8) & 0xFF;
	proto->data[proto->data_len++] = machine->rfid.money & 0xFF;
}


static void STATUSREPORTER_timeout(){
	timeout_flag = true;
}
