/*
 * protocol.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <App/protocol.h>
#include <Lib/utils/utils_buffer.h>

#include <string.h>

#include <Hal/uart.h>
#include <Hal/timer.h>

#define PROTOCOL_UART 	UART_3
#define PROTOCOL_BUFFER_MAX		128

static void PROTOCOL_timerInterrupt1ms(void);
static bool PROTOCOL_parse(uint8_t * data, size_t data_len, PROTOCOL_t *proto);
static uint8_t PROTOCOL_calCheckSum(uint8_t *data, uint8_t data_len);
static void PROTOCOL_serialize(PROTOCOL_t * proto, uint8_t *data, size_t * data_len);

static PROTOCOL_t PROTOCOL_message;
static utils_buffer_t PROTOCOL_rxMessage;
static uint8_t rx_buffer[RX_BUFFER_MAX_LENGTH];
static uint8_t rx_buffer_len = 0;
static uint8_t tx_buffer[TX_BUFFER_MAX_LENGTH];
static uint32_t rx_time_cnt = 0;
static bool rx_timeout = false;

static void PROTOCOL_onUARTCallback(void);

void PROTOCOL_init(void){
	TIMER_attach_intr_1ms(PROTOCOL_timerInterrupt1ms);
	utils_buffer_init(&PROTOCOL_rxMessage, sizeof(PROTOCOL_t));
	UART_set_receive_callback(PROTOCOL_UART, PROTOCOL_onUARTCallback);
}

void PROTOCOL_run(void){

}

void PROTOCOL_send(PROTOCOL_t *proto){
	size_t tx_len;
	PROTOCOL_serialize(proto, tx_buffer, &tx_len);
	UART_send(PROTOCOL_UART, tx_buffer, tx_len);
}

bool PROTOCOL_receive(PROTOCOL_t * proto){
	if(utils_buffer_is_available(&PROTOCOL_rxMessage)){
		return utils_buffer_pop(&PROTOCOL_rxMessage, proto);
	}
	return false;
}

static void PROTOCOL_timerInterrupt1ms(void){
	if(rx_time_cnt > 0){
		rx_time_cnt--;
		if(rx_time_cnt == 0){
			rx_timeout = true;
		}
	}
}

static bool PROTOCOL_parse(uint8_t * data, size_t data_len, PROTOCOL_t *proto){
	if(data[0] != START_BYTE){
		return false;
	}
	uint8_t dataL = data[2];
	uint8_t expectedChecksum = PROTOCOL_calCheckSum(&data[3], dataL);
	uint8_t checksum = data[3 + dataL];
	if(expectedChecksum != checksum){
		return false;
	}
	if(data[4 + dataL] != STOP_BYTE){
		return false;
	}
	if(data_len < 5 + dataL){
		return false;
	}
	proto->protocol_id = data[1];
	memcpy(proto->data, &data[3], dataL);
	proto->data_len = dataL;
	return true;
}

static uint8_t PROTOCOL_calCheckSum(uint8_t *data, uint8_t data_len){
	if(data_len == 0){
		return 0;
	}
	uint8_t checkSum = data[0];
	for (int var = 1; var < data_len; ++var) {
		checkSum ^= data[var];
	}
	return checkSum;
}

static void PROTOCOL_serialize(PROTOCOL_t * proto, uint8_t *data, size_t * data_len){
	uint8_t data_len_temp = 0;
	data[data_len_temp++] = START_BYTE;
	data[data_len_temp++] = proto->protocol_id;
	data[data_len_temp++] = proto->data_len;
	for (int var = 0; var < proto->data_len; ++var) {
		data[data_len_temp++] = proto->data[var];
	}
	data[data_len_temp++] = PROTOCOL_calCheckSum(proto->data, proto->data_len);
	data[data_len_temp++] = STOP_BYTE;
	*data_len = data_len_temp;
}

static void PROTOCOL_onUARTCallback(void){
	if(UART_receive_available(PROTOCOL_UART)){
		rx_time_cnt = RX_TIMEOUT_MS;
		rx_timeout = false;
		rx_buffer[rx_buffer_len++] = UART_receive_data(PROTOCOL_UART);
		if(PROTOCOL_parse(rx_buffer, rx_buffer_len, &PROTOCOL_message)){
			utils_buffer_push(&PROTOCOL_rxMessage, &PROTOCOL_message);
			rx_buffer_len = 0;
		}else if (rx_buffer_len > PROTOCOL_BUFFER_MAX){
			// Buffer is too big, but cannot parse -> Cleaning up
			rx_buffer_len = 0;
		}
	}
	if(rx_timeout){
		rx_buffer_len = 0;
	}
}
