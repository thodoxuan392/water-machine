/*
 * protocol.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_PROTOCOL_H_
#define INC_APP_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define START_BYTE 0x78
#define STOP_BYTE 0x79
#define DATA_MAX_LENGTH 	255
#define TX_BUFFER_MAX_LENGTH	512
#define RX_BUFFER_MAX_LENGTH	1024
#define RX_TIMEOUT_MS				100			// 100ms

typedef struct {
	uint8_t protocol_id;
	uint8_t data_len;
	uint8_t data[DATA_MAX_LENGTH];
}PROTOCOL_t;

typedef enum {
	PROTOCOL_ID_CONFIG = 0x01,
	PROTOCOL_ID_CONFIG_ACK = 0x02,
	PROTOCOL_ID_COMMAND_OPEN_VAN = 0x41,
	PROTOCOL_ID_COMMAND_OPEN_VAN_ACK = 0x42,
	PROTOCOL_ID_COMMAND_PLAY_AUDIO = 0x43,
	PROTOCOL_ID_COMMAND_PLAY_AUDIO_ACK = 0x44,
	PROTOCOL_ID_COMMAND_UPDATE_RFID = 0x45,
	PROTOCOL_ID_COMMAND_UPDATE_RFID_ACK = 0x46,
	PROTOCOL_ID_STATUS = 0x81,
	PROTOCOL_ID_RFID_DETECTED = 0xC1,
}PROTOCOL_Id_t;

void PROTOCOL_init(void);
void PROTOCOL_run(void);
void PROTOCOL_send(PROTOCOL_t *proto);
bool PROTOCOL_receive(PROTOCOL_t * proto);


#endif /* INC_APP_PROTOCOL_H_ */
