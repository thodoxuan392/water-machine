/*
 * commandhandler.h
 *
 *  Created on: Jun 2, 2023
 *      Author: xuanthodo
 */

#ifndef INC_APP_COMMANDHANDLER_H_
#define INC_APP_COMMANDHANDLER_H_

#include "stdio.h"
#include "stdbool.h"


enum {
	RESULT_SUCCESS = 0x00,
	RESULT_FAILED = 0x01,

	// For RFID
	RESULT_RFID_ERROR_NOT_AVAILABLE = 0x70,
	RESULT_RFID_ERROR_INVALID_FORMAT = 0x71,
	RESULT_RFID_ERROR_ID_NOT_MATCHED = 0x72,
	RESULT_RFID_ERROR_AUTHEN_FAILED = 0x73,
	RESULT_RFID_ERROR_CANNOT_WRITE = 0x74,
};

bool COMMANDHANDLER_init();
void COMMANDHANDLER_run();
void COMMANDHANDLER_sendCommandOpenVanResult(uint8_t machineId, uint8_t result);

#endif /* INC_APP_COMMANDHANDLER_H_ */
