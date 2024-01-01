/*
 * vanmanager.c
 *
 *  Created on: Dec 31, 2023
 *      Author: xuanthodo
 */


#include <DeviceManager/vanmanager.h>

#include <Device/solenoid.h>
#include <Device/waterflow.h>

#include <Hal/timer.h>

#define VANMANAGER_FLUSH_TIMER_PERIOD_MS	100
#define VANMANAGER_OPEN_VAN_TIMEOUT_MS		( 2 * 60 * 1000)

typedef enum {
	VANMANAGER_IDLE,
	VANMANAGER_OPEN_VAN,
	VANMANAGER_WAIT_FOR_OPEN_VAN_DONE
}VANMANAGER_State;

typedef struct {
	SOLENOID_Id solenoid_id;
	WATERFLOW_Id_t waterflow_id;
	bool openVanRequested;
	uint16_t volumeRequested;
	uint16_t volumeFlushed;
	VANMANAGER_State state;

	uint32_t flushTimerCnt;		// Calculate Volume flash every timer cnt

	uint32_t openVanTimeCnt;
	bool openVanTimeoutFlag;

}VANMANAGER_Handle;

static VANMANAGER_Handle VANMANAGER_handleTable[] = {
	[SOLENOID_ID_1] = {
		.solenoid_id = SOLENOID_ID_1,
		.waterflow_id = WATERFLOW_ID_1,
		.state = VANMANAGER_IDLE,
	},
	[SOLENOID_ID_2] = {
		.solenoid_id = SOLENOID_ID_2,
		.waterflow_id = WATERFLOW_ID_2,
		.state = VANMANAGER_IDLE,
	},
	[SOLENOID_ID_3] = {
		.solenoid_id = SOLENOID_ID_3,
		.waterflow_id = WATERFLOW_ID_3,
		.state = VANMANAGER_IDLE,
	}
};
static VANMANAGER_onOpenVanCompletedCallback VANMANAGER_onCompletedCallback = NULL;

static void VANMANAGER_interrupt1ms(void);

static void VANMANAGER_runByHandle(VANMANAGER_Handle*);
static void VANMANAGER_runIdle(VANMANAGER_Handle*);
static void VANMANAGER_runOpenVan(VANMANAGER_Handle*);
static void VANMANAGER_runWaitForOpenVanDone(VANMANAGER_Handle*);

static uint32_t VANMANAGER_calVolumeFlushedInTime(SOLENOID_Id id,  uint32_t timeMs);


void VANMANAGER_init(void){
	TIMER_attach_intr_1ms(VANMANAGER_interrupt1ms);
}

void VANMANAGER_run(void){
	for (int id = 0; id < SOLENOID_ID_MAX; ++id) {
		VANMANAGER_runByHandle(&VANMANAGER_handleTable[id]);
	}
}

void VANMANAGER_setOnOpenVanCompletedCallback(VANMANAGER_onOpenVanCompletedCallback callback){
	VANMANAGER_onCompletedCallback = callback;
}

bool VANMANAGER_openVan(SOLENOID_Id id, uint16_t volume){
	if(VANMANAGER_handleTable[id].state!= VANMANAGER_IDLE){
		return false;
	}
	VANMANAGER_handleTable[id].openVanRequested = true;
	VANMANAGER_handleTable[id].volumeRequested = volume;
	return true;
}

static void VANMANAGER_runByHandle(VANMANAGER_Handle* handle){
	switch (handle->state) {
		case VANMANAGER_IDLE:
			VANMANAGER_runIdle(handle);
			break;
		case VANMANAGER_OPEN_VAN:
			VANMANAGER_runOpenVan(handle);
			break;
		case VANMANAGER_WAIT_FOR_OPEN_VAN_DONE:
			VANMANAGER_runWaitForOpenVanDone(handle);
			break;
		default:
			break;
	}
}


static void VANMANAGER_runIdle(VANMANAGER_Handle* handle){
	if(handle->openVanRequested){
		handle->state = VANMANAGER_OPEN_VAN;
	}
}

static void VANMANAGER_runOpenVan(VANMANAGER_Handle* handle){
	SOLENOID_set(handle->solenoid_id, true);
	handle->flushTimerCnt = VANMANAGER_FLUSH_TIMER_PERIOD_MS;
	handle->volumeFlushed = 0;
	handle->openVanTimeCnt = VANMANAGER_OPEN_VAN_TIMEOUT_MS;
	handle->openVanTimeoutFlag = false;
	handle->state = VANMANAGER_WAIT_FOR_OPEN_VAN_DONE;
}

static void VANMANAGER_runWaitForOpenVanDone(VANMANAGER_Handle* handle){
	if(handle->volumeFlushed >= handle->volumeRequested){
		SOLENOID_set(handle->solenoid_id, false);
		handle->openVanRequested = false;
		handle->state = VANMANAGER_IDLE;
		if(VANMANAGER_onCompletedCallback){
			// Complete success callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id,  true);
		}
	}
	if(handle->openVanTimeoutFlag){
		SOLENOID_set(handle->solenoid_id, false);
		handle->openVanRequested = false;
		handle->state = VANMANAGER_IDLE;
		if(VANMANAGER_onCompletedCallback){
			// Complete failed callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id, false);
		}
	}
}

static uint32_t VANMANAGER_calVolumeFlushedInTime(SOLENOID_Id id,  uint32_t timeMs){
	uint32_t waterFlowInCCOverMs = WATERFLOW_get(VANMANAGER_handleTable[id].waterflow_id) * timeMs / 60;
	return waterFlowInCCOverMs;
}

static void VANMANAGER_interrupt1ms(void){
	for (int id = 0; id < SOLENOID_ID_MAX; ++id) {
		// Flash timer counter
		if(VANMANAGER_handleTable[id].openVanRequested){
			if(VANMANAGER_handleTable[id].flushTimerCnt > 0){
				VANMANAGER_handleTable[id].flushTimerCnt--;
				if(VANMANAGER_handleTable[id].flushTimerCnt == 0){
					VANMANAGER_handleTable[id].flushTimerCnt = VANMANAGER_FLUSH_TIMER_PERIOD_MS;
					VANMANAGER_handleTable[id].volumeFlushed += VANMANAGER_calVolumeFlushedInTime(id, VANMANAGER_FLUSH_TIMER_PERIOD_MS);
				}
			}
		}

		// Timeout when cannot reach to volume requested
		if(VANMANAGER_handleTable[id].openVanTimeCnt > 0){
			VANMANAGER_handleTable[id].openVanTimeCnt --;
			if(VANMANAGER_handleTable[id].openVanTimeCnt == 0){
				VANMANAGER_handleTable[id].openVanTimeoutFlag = true;
			}
		}
	}
}
