/*
 * commandhandler.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */

#include "main.h"
#include "string.h"
#include "config.h"

#include "App/commandhandler.h"
#include "App/protocol.h"
#include "App/statemachine.h"

#include "Lib/utils/utils_logger.h"

enum {
	COMMANDHANDLE_IDLE,
	COMMANDHANDLE_EXECUTE
};

enum {
	COMMAND_RESET,
	COMMAND_OTA,
	COMMAND_RESET_DEFAULT_CONFIG,
	COMMAND_DELETE_TOTAL_CARD,
	COMMAND_DELETE_TOTAL_AMOUNT
};


static void COMMANDHANDLER_handleConfig(PROTOCOL_t *proto);
static void COMMANDHANDLER_handleCommandOpenVan(PROTOCOL_t *proto);
static void COMMANDHANDLER_handleCommandPlayAudio(PROTOCOL_t *proto);
static void COMMANDHANDLER_handleCommandUpdateRfid(PROTOCOL_t *proto);

// ACK
static void COMMANDHANDLER_sendConfigACK(uint8_t machineId);
static void COMMANDHANDLER_sendCommandOpenVanACK(uint8_t machineId);
static void COMMANDHANDLER_sendCommandPlayAudioACK(uint8_t machineId);
static void COMMANDHANDLER_sendCommandUpdateRfidACK(uint8_t machineId);



static PROTOCOL_t receive_message;

bool COMMANDHANDLER_init(){

}

void COMMANDHANDLER_run(){
	if(PROTOCOL_receive(&receive_message)){
		switch (receive_message.protocol_id) {
			case PROTOCOL_ID_CONFIG:
				COMMANDHANDLER_handleConfig(&receive_message);
				break;
			case PROTOCOL_ID_COMMAND_OPEN_VAN:
				COMMANDHANDLER_handleCommandOpenVan(&receive_message);
				break;
			case PROTOCOL_ID_COMMAND_PLAY_AUDIO:
				COMMANDHANDLER_handleCommandPlayAudio(&receive_message);
				break;
			case PROTOCOL_ID_COMMAND_UPDATE_RFID:
				COMMANDHANDLER_handleCommandUpdateRfid(&receive_message);
				break;
			default:
				break;
		}
	}
}
static void COMMANDHANDLER_handleConfig(PROTOCOL_t *proto){
	if(proto->data_len != 2){
		utils_log_error("HandleConfig failed: Invalid data_len %d, expected 2\r\n", proto->data_len);
		return;
	}
	uint8_t machineId = proto->data[0];
	uint8_t maximumWaterFlowAllowed = proto->data[1];
	// Send ACK
	COMMANDHANDLER_sendConfigACK(machineId);
}
static void COMMANDHANDLER_handleCommandOpenVan(PROTOCOL_t *proto){
	if(proto->data_len != 2){
		utils_log_error("HandleCommandOpenVan failed: Invalid data_len %d, expected 2\r\n", proto->data_len);
		return;
	}
	uint8_t machineId = proto->data[0];
	bool enable = (bool)proto->data[1];
	// Send ACK
	COMMANDHANDLER_sendCommandOpenVanACK(machineId);
	// Handle
	STATEMACHINE_openVAN(machineId, enable);
}
static void COMMANDHANDLER_handleCommandPlayAudio(PROTOCOL_t *proto){
	if(proto->data_len != 2){
		utils_log_error("HandleCommandPlayAudio failed: Invalid data_len %d, expected 2\r\n", proto->data_len);
		return;
	}
	uint8_t machineId = proto->data[0];
	bool enable = (bool)proto->data[1];
	// Send ACK
	COMMANDHANDLER_sendCommandPlayAudioACK(machineId);
	// Handle
	STATEMACHINE_openVAN(machineId, enable);
}
static void COMMANDHANDLER_handleCommandUpdateRfid(PROTOCOL_t *proto){
	uint8_t machineId = proto->data[0];
	bool enable = (bool)proto->data[1];
	// Send ACK
	COMMANDHANDLER_sendCommandUpdateRfidACK(machineId);
	// Handle
	STATEMACHINE_openVAN(machineId, enable);
}

static void COMMANDHANDLER_sendConfigACK(uint8_t machineId){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_CONFIG_ACK;
	protocol.data_len = 1;
	protocol.data[0] = machineId;
	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendCommandOpenVanACK(uint8_t machineId){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_COMMAND_OPEN_VAN_ACK;
	protocol.data_len = 1;
	protocol.data[0] = machineId;
	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendCommandPlayAudioACK(uint8_t machineId){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_COMMAND_PLAY_AUDIO_ACK;
	protocol.data_len = 1;
	protocol.data[0] = machineId;
	PROTOCOL_send(&protocol);
}

static void COMMANDHANDLER_sendCommandUpdateRfidACK(uint8_t machineId){
	PROTOCOL_t protocol;
	protocol.protocol_id = PROTOCOL_ID_COMMAND_UPDATE_RFID_ACK;
	protocol.data_len = 1;
	protocol.data[0] = machineId;
	PROTOCOL_send(&protocol);
}

