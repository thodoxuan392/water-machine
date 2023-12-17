/*
 * rfid.h
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICE_RFID_H_
#define INC_DEVICE_RFID_H_

#include <stdbool.h>
#include <stdint.h>

#define RFID_ID_MAX_LENGTH	15

typedef enum {
	RFID_ID_1 = 0,
	RFID_ID_2
}RFID_Id_t;

typedef struct {
	uint8_t id[RFID_ID_MAX_LENGTH];
	uint8_t id_len;
	uint16_t money;
}RFID_t;


bool RFID_init(void);
bool RFID_get(RFID_Id_t id, RFID_t *);
bool RFID_set(RFID_Id_t id, RFID_t *);

#endif /* INC_DEVICE_RFID_H_ */
