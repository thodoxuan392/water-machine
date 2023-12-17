/*
 * protocol.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <App/protocol.h>

void PROTOCOL_init(void){

}

void PROTOCOL_run(void){

}

void PROTOCOL_send(PROTOCOL_t *proto){

}

bool PROTOCOL_receive(PROTOCOL_t * proto){

}

uint8_t PROTOCOL_calCheckSum(uint8_t *data, uint8_t data_len){
	if(data_len == 0){
		return 0;
	}
	uint8_t checkSum = data[0];
	for (int var = 1; var < data_len; ++var) {
		checkSum ^= data[var];
	}
	return checkSum;
}

void PROTOCOL_serialize(PROTOCOL_t * proto, uint8_t *data, size_t * data_len){
	uint8_t data_len_temp = 0;
	data[data_len_temp++] = START_BYTE;
	data[data_len_temp++] = proto->protocol_id;
	data[data_len_temp++] = proto->data_len;
	for (int var = 0; var < proto->data_len; ++var) {
		data[data_len_temp++] = proto->data[var];
	}
	data[data_len_temp++] = PROTOCOL_calCheckSum(proto->data, proto->data_len);
	data[data_len_temp++] = STOP_BYTE;
}
