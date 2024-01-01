/*
 * sound.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/sound.h>

#include <utils/utils_logger.h>

#include <Hal/uart.h>
#include <Hal/timer.h>

#define SOUND_INITIALIZATION_TIME_MS			3000
#define SOUND_DELAY_BETWEEN_PLAYING_TRACKS_MS	100
#define SOUND_COMMAND_TIMEOUT					1000
#define SOUND_RX_BUFFER_LEN			128

#define SOUND_START_BYTE		0x7E
#define SOUND_STOP_BYTE			0xEF

#define SOUND_VOLUME_DEFAULT					50
#define SOUND_FOLDER_INDEX_DEFAULT				0x01

/**
 * Command
 */
#define SOUND_COMMAND_NEXT 						0x01
#define SOUND_COMMAND_PREVIOUS 					0x02
#define SOUND_COMMAND_SPECIFY_TRACKING 			0x03
#define SOUND_COMMAND_INCREASE_VOLUME 			0x04
#define SOUND_COMMAND_DECREASE_VOLUME 			0x05
#define SOUND_COMMAND_SPECIFY_VOLUME 			0x06
#define SOUND_COMMAND_SPECIFY_EQ 				0x07
#define SOUND_COMMAND_SPECIFY_PLAYBACK_MODE 	0x08
#define SOUND_COMMAND_SPECIFY_PLAYBACK_SOURCE 	0x09
#define SOUND_COMMAND_ENTER_STANDBY 			0x0A
#define SOUND_COMMAND_NORMAL_WORKING 			0x0B
#define SOUND_COMMAND_RESET_MODULE 				0x0C
#define SOUND_COMMAND_PLAYBACK 					0x0D
#define SOUND_COMMAND_PAUSE 					0x0E
#define SOUND_COMMAND_SPECIFY_FOL_PLAYBACK 		0x0F
#define SOUND_COMMAND_VOLUME_ADJUST 			0x10
#define SOUND_COMMAND_REPEAT_PLAY 				0x11


/**
 * Query
 */

#define SOUND_QUERY_UDISK_STAY					0x3C
#define SOUND_QUERY_TFCARD_STAY					0x3D
#define SOUND_QUERY_FLASH_STAY					0x3E
#define SOUND_QUERY_INITIALIZATION_PARAM		0x3F
#define SOUND_QUERY_RETURN_ERROR_RETRANSMIT		0x40
#define SOUND_QUERY_REPLY						0x41
#define SOUND_QUERY_QUERY_CURRENT_STATUS		0x42
#define SOUND_QUERY_QUERY_CURRENT_VOLUME		0x43
#define SOUND_QUERY_QUERY_CURRENT_EQ			0x44
#define SOUND_QUERY_CURRENT_PLAYBACK_MODE		0x45
#define SOUND_QUERY_QUERY_CURRENT_SW_VERSION	0x46
#define SOUND_QUERY_QUERY_TOTAL_TF_FILE			0x47
#define SOUND_QUERY_QUERY_TOTAL_UDISK_FILE		0x48
#define SOUND_QUERY_QUERY_TOTAL_FLASH_FILE		0x49
#define SOUND_QUERY_QUERY_KEEP_ON				0x4A
#define SOUND_QUERY_QUERY_CURRENT_TRACK_TF		0x4B
#define SOUND_QUERY_QUERY_CURRENT_TRACK_UDISK	0x4C
#define SOUND_QUERY_QUERY_CURRENT_TRACK_FLASH	0x4D

typedef struct {
	uint8_t start_byte;
	uint8_t version;
	uint8_t len;
	uint8_t command;
	uint8_t feedback;
	uint8_t param1;
	uint8_t param2;
	uint8_t checksum[2];
	uint8_t stop_byte;
}SOUND_Protocol;

typedef struct {
	SOUND_Id id;
	UART_id_t uartId;

	uint32_t timeoutCnt;
	bool timeoutFlag;
	uint8_t rxBuffer[SOUND_RX_BUFFER_LEN];
	uint32_t rxBufferLen;
	bool error;
}SOUND_Handle;

static void SOUND_timerInterrupt1ms(void);

static void SOUND_clearRxBuffer(SOUND_Handle * handle);
static void SOUND_sendProtocol(SOUND_Handle * handle, SOUND_Protocol * protocol);
static bool SOUND_waitProtocolFeedback(SOUND_Handle * handle, SOUND_Protocol * protocol, uint32_t timeout);
static bool SOUND_parseProtocol(uint8_t *data, uint32_t data_len, SOUND_Protocol * protocol);

static bool SOUND_initById(SOUND_Id id);
static bool SOUND_reset(SOUND_Handle * handle);
static bool SOUND_setDefaultEQ(SOUND_Handle * handle);
static bool SOUND_setDefaultPlaybackMode(SOUND_Handle * handle);
static bool SOUND_setDefaultVolume(SOUND_Handle * handle);
static bool SOUND_setDefaultPlaybackSource(SOUND_Handle * handle);
static bool SOUND_playSpecifyFile(SOUND_Handle * handle,uint8_t file);


static SOUND_Handle SOUND_handleTable[] = {
		[SOUND_ID_1] = {
			.id = SOUND_ID_1,
			.uartId = UART_1
		},
		[SOUND_ID_2] = {
			.id = SOUND_ID_2,
			.uartId = UART_3
		},
		[SOUND_ID_3] = {
			.id = SOUND_ID_2,
			.uartId = UART_4
		},
};

bool SOUND_init(void){
	TIMER_attach_intr_1ms(SOUND_timerInterrupt1ms);
	for (int id = 0; id < SOUND_ID_MAX; ++id) {
		SOUND_initById(id);
	}
}

bool SOUND_play(SOUND_Id id, SOUND_File file){
	SOUND_playSpecifyFile(&SOUND_handleTable[id], file);
}

bool SOUND_isError(SOUND_Id id){
	return SOUND_handleTable[id].error;
}

static bool SOUND_initById(SOUND_Id id){
	if(!SOUND_reset(&SOUND_handleTable[id])){
		utils_log_error("Couldn't reset Sound module\r\n");
	}
	if(!SOUND_setDefaultEQ(&SOUND_handleTable[id])){
		utils_log_error("Couldn't set Sound EQ\r\n");
	}
	if(!SOUND_setDefaultPlaybackMode(&SOUND_handleTable[id])){
		utils_log_error("Couldn't set Sound playback mode\r\n");
	}
	if(!SOUND_setDefaultVolume(&SOUND_handleTable[id])){
		utils_log_error("Couldn't reset Sound volume\r\n");
	}
	if(!SOUND_setDefaultPlaybackSource(&SOUND_handleTable[id])){
		utils_log_error("Couldn't reset Sound playback source\r\n");
	}
}

static void SOUND_timerInterrupt1ms(void){
	for (int id = 0; id < SOUND_ID_MAX; ++id) {
		if(SOUND_handleTable[id].timeoutCnt > 0){
			SOUND_handleTable[id].timeoutCnt--;
			if(SOUND_handleTable[id].timeoutCnt == 0){
				SOUND_handleTable[id].timeoutFlag = true;
			}
		}
	}
}

static void SOUND_clearRxBuffer(SOUND_Handle * handle){
	handle->rxBufferLen = 0;
}

static void SOUND_sendProtocol(SOUND_Handle * handle, SOUND_Protocol * protocol){
	protocol->start_byte = SOUND_START_BYTE;
	protocol->version = 0xFF;
	protocol->len = 0x06;
	protocol->stop_byte = SOUND_STOP_BYTE;
	UART_send(handle->uartId, (uint8_t*)protocol, sizeof(SOUND_Protocol));
}

static bool SOUND_waitProtocolFeedback(SOUND_Handle * handle, SOUND_Protocol * protocol, uint32_t timeout){
	handle->timeoutCnt = timeout;
	handle->timeoutFlag = false;
	SOUND_clearRxBuffer(handle);
	while(1){
		if(UART_receive_available(handle->uartId)){
			handle->rxBuffer[handle->rxBufferLen++] = UART_receive_data(handle->uartId);
			if(SOUND_parseProtocol(handle->rxBuffer, handle->rxBufferLen, protocol)){
				return true;
			}
		}
		if(handle->timeoutFlag){
			return false;
		}
	}

}

static bool SOUND_parseProtocol(uint8_t *data, uint32_t data_len, SOUND_Protocol * protocol){
	if(data_len < 10){
		return false;
	}
	if(SOUND_START_BYTE != data[0] || SOUND_STOP_BYTE != data[9]){
		return false;
	}
	return true;
}

static bool SOUND_reset(SOUND_Handle * handle){
	SOUND_Protocol protocol;
	protocol.command = SOUND_COMMAND_RESET_MODULE;
	protocol.feedback = 0x01;
	// Send Protocol
	SOUND_sendProtocol(handle, &protocol);
	// Wait for Module reset
	if(!SOUND_waitProtocolFeedback(handle, &protocol, SOUND_INITIALIZATION_TIME_MS)){
		return false;
	}
	handle->error = false;
	if(protocol.command == SOUND_QUERY_RETURN_ERROR_RETRANSMIT){
		handle->error = true;
		return false;
	}
	return true;
}

static bool SOUND_setDefaultEQ(SOUND_Handle * handle){
	SOUND_Protocol protocol;
	protocol.command = SOUND_COMMAND_SPECIFY_EQ;
	protocol.feedback = 0x00;
	protocol.param1 = 0x00;
	protocol.param2 = 0x00;

	// Send Protocol
	SOUND_sendProtocol(handle, &protocol);
	// Wait for Module reset
	if(!SOUND_waitProtocolFeedback(handle, &protocol, SOUND_COMMAND_TIMEOUT)){
		return false;
	}
	handle->error = false;
	if(protocol.command == SOUND_QUERY_RETURN_ERROR_RETRANSMIT){
		handle->error = true;
		return false;
	}
	return true;
}

static bool SOUND_setDefaultPlaybackMode(SOUND_Handle * handle){
	SOUND_Protocol protocol;
	protocol.command = SOUND_COMMAND_SPECIFY_PLAYBACK_MODE;
	protocol.feedback = 0x00;
	protocol.param1 = 0x00;
	protocol.param2 = 0x02;
	// Send Protocol
	SOUND_sendProtocol(handle, &protocol);
	// Wait for Module reset
	if(!SOUND_waitProtocolFeedback(handle, &protocol, SOUND_COMMAND_TIMEOUT)){
		return false;
	}
	handle->error = false;
	if(protocol.command == SOUND_QUERY_RETURN_ERROR_RETRANSMIT){
		handle->error = true;
		return false;
	}
	return true;
}

static bool SOUND_setDefaultVolume(SOUND_Handle * handle){
	SOUND_Protocol protocol;
	protocol.command = SOUND_COMMAND_SPECIFY_VOLUME;
	protocol.feedback = 0x00;
	protocol.param1 = 0x00;
	protocol.param2 = 0x0F;
	// Send Protocol
	SOUND_sendProtocol(handle, &protocol);
	// Wait for Module reset
	if(!SOUND_waitProtocolFeedback(handle, &protocol, SOUND_COMMAND_TIMEOUT)){
		return false;
	}
	handle->error = false;
	if(protocol.command == SOUND_QUERY_RETURN_ERROR_RETRANSMIT){
		handle->error = true;
		return false;
	}
	return true;
}

static bool SOUND_setDefaultPlaybackSource(SOUND_Handle * handle){
	SOUND_Protocol protocol;
	protocol.command = SOUND_COMMAND_SPECIFY_PLAYBACK_SOURCE;
	protocol.feedback = 0x00;
	protocol.param1 = 0x00;
	protocol.param2 = 0x02;
	// Send Protocol
	SOUND_sendProtocol(handle, &protocol);
	// Wait for Module reset
	if(!SOUND_waitProtocolFeedback(handle, &protocol, SOUND_COMMAND_TIMEOUT)){
		return false;
	}
	handle->error = false;
	if(protocol.command == SOUND_QUERY_RETURN_ERROR_RETRANSMIT){
		handle->error = true;
		return false;
	}
	return true;
}

static bool SOUND_playSpecifyFile(SOUND_Handle * handle, uint8_t file){
	SOUND_Protocol protocol;
	protocol.command = SOUND_COMMAND_SPECIFY_FOL_PLAYBACK;
	protocol.feedback = 0x00;
	protocol.param1 = SOUND_FOLDER_INDEX_DEFAULT;
	protocol.param2 = file;
	// Send Protocol
	SOUND_sendProtocol(handle, &protocol);
	// Wait for Module reset
	if(!SOUND_waitProtocolFeedback(handle, &protocol, SOUND_COMMAND_TIMEOUT)){
		return false;
	}
	handle->error = false;
	if(protocol.command == SOUND_QUERY_RETURN_ERROR_RETRANSMIT){
		handle->error = true;
		return false;
	}
	return true;
}
