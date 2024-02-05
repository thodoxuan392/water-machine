/*
 * uart.h
 *
 *  Created on: May 10, 2023
 *      Author: xuanthodo
 */

#ifndef INC_HAL_UART_H_
#define INC_HAL_UART_H_

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"


typedef enum {
	UART_2,
	UART_4,
	UART_MAX
}UART_id_t;

bool UART_init();
bool UART_send(UART_id_t id, uint8_t *data , size_t len);
bool UART_receive_available(UART_id_t id);
uint16_t UART_receive_data(UART_id_t id);
void UART_set_receive_callback(UART_id_t id, void (*callback)(void));
void UART_clear_buffer(UART_id_t id);
void UART_test();


#endif /* INC_HAL_UART_H_ */
