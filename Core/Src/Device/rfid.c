/*
 * rfid.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/rfid.h>

#include <PN532.h>
#include <PN532_I2C.h>


void RFID_init(void){
	PN532_init(PN532_I2C_getInterface());
	PN532_begin();
}

bool RFID_get(RFID_Id_t id, RFID_t * rfid){

}

bool RFID_isPlaced(RFID_Id_t id){

}

bool RFID_set(RFID_Id_t id, RFID_t * rfid){

}
