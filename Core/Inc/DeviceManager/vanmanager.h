/*
 * vanmanager.h
 *
 *  Created on: Dec 31, 2023
 *      Author: xuanthodo
 */

#ifndef INC_DEVICEMANAGER_VANMANAGER_H_
#define INC_DEVICEMANAGER_VANMANAGER_H_

#include <stdbool.h>
#include <stdint.h>

#include <Device/solenoid.h>

typedef void (*VANMANAGER_onOpenVanCompletedCallback)(uint8_t solenoidId, bool success);

void VANMANAGER_init(void);
void VANMANAGER_run(void);
void VANMANAGER_setOnOpenVanCompletedCallback(VANMANAGER_onOpenVanCompletedCallback callback);
bool VANMANAGER_openVan(SOLENOID_Id id, uint16_t volume);
bool VANMANAGER_cancelOpenVan(SOLENOID_Id id);

#endif /* INC_DEVICEMANAGER_VANMANAGER_H_ */
