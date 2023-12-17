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
static void COMMANDHANDLER_handleConfig(PROTOCOL_t *proto){}
static void COMMANDHANDLER_handleCommandOpenVan(PROTOCOL_t *proto){}
static void COMMANDHANDLER_handleCommandPlayAudio(PROTOCOL_t *proto){}
static void COMMANDHANDLER_handleCommandUpdateRfid(PROTOCOL_t *proto){}


