/*
 * vanmanager.c
 *
 *  Created on: Dec 31, 2023
 *      Author: xuanthodo
 */


#include <DeviceManager/vanmanager.h>

#include <Device/solenoid.h>
#include <Device/waterflow.h>
#include <Device/placedpoint.h>

#include <Hal/timer.h>

#define VANMANAGER_FLUSH_TIMER_PERIOD_MS	100
#define VANMANAGER_OPEN_VAN_TIMEOUT_MS		( 2 * 60 * 1000)

typedef enum {
	VANMANAGER_IDLE,
	VANMANAGER_OPEN_VAN,
	VANMANAGER_WAIT_FOR_OPEN_VAN_DONE,
	VANMANAGER_WAIT_FOR_PLACED_POINT,
}VANMANAGER_State;

typedef struct {
	SOLENOID_Id solenoid_id;
	WATERFLOW_Id_t waterflow_id;
	PLACEDPOINT_Id_t placedpoint_id;
	bool openVanRequested;
	bool cancelOpenVanRequested;
	bool isOpening;

	uint16_t volumeRequested;
	uint16_t volumeFlushed;

	uint32_t pulseRequested;

	VANMANAGER_State state;

	uint32_t flushTimerCnt;		// Calculate Volume flash every timer cnt

	uint32_t openVanTimeCnt;
	bool openVanTimeoutFlag;

}VANMANAGER_Handle;

static VANMANAGER_Handle VANMANAGER_handleTable[SOLENOID_ID_MAX] = {
	[SOLENOID_ID_1] = {
		.solenoid_id = SOLENOID_ID_1,
		.waterflow_id = WATERFLOW_ID_1,
		.placedpoint_id = PLACEDPOINT_ID_1,
		.state = VANMANAGER_IDLE,
	},
	[SOLENOID_ID_2] = {
		.solenoid_id = SOLENOID_ID_2,
		.waterflow_id = WATERFLOW_ID_2,
		.placedpoint_id = PLACEDPOINT_ID_2,
		.state = VANMANAGER_IDLE,
	},
	[SOLENOID_ID_3] = {
		.solenoid_id = SOLENOID_ID_3,
		.waterflow_id = WATERFLOW_ID_3,
		.placedpoint_id = PLACEDPOINT_ID_3,
		.state = VANMANAGER_IDLE,
	}
};
static void (*VANMANAGER_onCompletedCallback)(uint8_t solenoidId, uint8_t success) = NULL;

static void VANMANAGER_interrupt1ms(void);

static void VANMANAGER_runByHandle(VANMANAGER_Handle*);
static void VANMANAGER_runIdle(VANMANAGER_Handle*);
static void VANMANAGER_runOpenVan(VANMANAGER_Handle*);
static void VANMANAGER_runWaitForOpenVanDone(VANMANAGER_Handle*);
static void VANMANAGER_runWaitForPlacedPoint(VANMANAGER_Handle*);

#if defined(WATERFLOW_SW_V1)
static uint32_t VANMANAGER_calVolumeFlushedInTime(SOLENOID_Id id,  uint32_t timeMs);
#endif

void VANMANAGER_init(void){
	TIMER_attach_intr_1ms(VANMANAGER_interrupt1ms);
}

void VANMANAGER_run(void){
	for (int id = 0; id < SOLENOID_ID_MAX; id++) {
		VANMANAGER_runByHandle(&VANMANAGER_handleTable[id]);
	}
}

void VANMANAGER_setOnOpenVanCompletedCallback( void (*callback)(uint8_t solenoidId, uint8_t success)){
	VANMANAGER_onCompletedCallback = callback;
}

bool VANMANAGER_openVan(SOLENOID_Id id, uint16_t volume){
	if(VANMANAGER_handleTable[id].state != VANMANAGER_IDLE){
		return false;
	}
	VANMANAGER_handleTable[id].openVanRequested = true;
	VANMANAGER_handleTable[id].volumeRequested = volume;
	VANMANAGER_handleTable[id].pulseRequested = WATERFLOW_getPulseByVolume(VANMANAGER_handleTable[id].waterflow_id, volume);
	return true;
}

bool VANMANAGER_cancelOpenVan(SOLENOID_Id id){
	if(VANMANAGER_handleTable[id].state != VANMANAGER_IDLE){
		VANMANAGER_handleTable[id].cancelOpenVanRequested = true;
	}
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
		case VANMANAGER_WAIT_FOR_PLACED_POINT:
			VANMANAGER_runWaitForPlacedPoint(handle);
			break;
		default:
			break;
	}
}


static void VANMANAGER_runIdle(VANMANAGER_Handle* handle){
	if(handle->openVanRequested){
		handle->openVanRequested = false;
		handle->flushTimerCnt = VANMANAGER_FLUSH_TIMER_PERIOD_MS;
		handle->volumeFlushed = 0;
		handle->openVanTimeCnt = VANMANAGER_OPEN_VAN_TIMEOUT_MS;
		handle->openVanTimeoutFlag = false;
		handle->state = VANMANAGER_WAIT_FOR_PLACED_POINT;
	}
}

static void VANMANAGER_runOpenVan(VANMANAGER_Handle* handle){
	SOLENOID_set(handle->solenoid_id, true);
	handle->isOpening = true;
	handle->state = VANMANAGER_WAIT_FOR_OPEN_VAN_DONE;
}

static void VANMANAGER_runWaitForOpenVanDone(VANMANAGER_Handle* handle){
#if defined(WATERFLOW_SW_V1)
	if(handle->volumeFlushed >= handle->volumeRequested){
		SOLENOID_set(handle->solenoid_id, false);
		handle->state = VANMANAGER_IDLE;
		handle->isOpening = false;
		if(VANMANAGER_onCompletedCallback){
			// Complete success callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id,  true);
		}
	}
#elif defined(WATERFLOW_SW_V2)
	if(WATERFLOW_getPulse(handle->waterflow_id) >= handle->pulseRequested){
		WATERFLOW_resetPulse(handle->waterflow_id);
		SOLENOID_set(handle->solenoid_id, false);
		handle->state = VANMANAGER_IDLE;
		handle->isOpening = false;
		if(VANMANAGER_onCompletedCallback){
			// Complete success callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id,  true);
		}
	}
#endif
	if(handle->openVanTimeoutFlag){
		SOLENOID_set(handle->solenoid_id, false);
		handle->state = VANMANAGER_IDLE;
		handle->isOpening = false;
		if(VANMANAGER_onCompletedCallback){
			// Complete failed callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id, false);
		}
	}
	if(handle->cancelOpenVanRequested){
		SOLENOID_set(handle->solenoid_id, false);
		handle->cancelOpenVanRequested = false;
		handle->isOpening = false;
		handle->state = VANMANAGER_IDLE;
		if(VANMANAGER_onCompletedCallback){
			// Complete failed callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id, false);
		}
	}
	if(!PLACEDPOINT_isPlaced(handle->placedpoint_id)){
		SOLENOID_set(handle->solenoid_id, false);
		handle->cancelOpenVanRequested = false;
		handle->state = VANMANAGER_WAIT_FOR_PLACED_POINT;
	}
}

static void VANMANAGER_runWaitForPlacedPoint(VANMANAGER_Handle* handle){
	if(PLACEDPOINT_isPlaced(handle->placedpoint_id)){
		if(!handle->isOpening){
			handle->state = VANMANAGER_OPEN_VAN;
		}else {
			SOLENOID_set(handle->solenoid_id, true);
			handle->state = VANMANAGER_WAIT_FOR_OPEN_VAN_DONE;
		}
	}
	if(handle->openVanTimeoutFlag){
		SOLENOID_set(handle->solenoid_id, false);
		handle->state = VANMANAGER_IDLE;
		handle->isOpening = false;
		if(VANMANAGER_onCompletedCallback){
			// Complete failed callback
			VANMANAGER_onCompletedCallback(handle->solenoid_id, false);
		}
	}
}

#if defined(WATERFLOW_SW_V1)
static uint32_t VANMANAGER_calVolumeFlushedInTime(SOLENOID_Id id,  uint32_t timeMs){
	uint32_t waterFlowInCCOverMs = WATERFLOW_get(VANMANAGER_handleTable[id].waterflow_id) * timeMs / 60;
	return waterFlowInCCOverMs;
}
#endif

static void VANMANAGER_interrupt1ms(void){
	for (int id = 0; id < SOLENOID_ID_MAX; ++id) {
#if defined(WATERFLOW_SW_V1)
		// Flash timer counter
		if(VANMANAGER_handleTable[id].isOpening){
			if(VANMANAGER_handleTable[id].flushTimerCnt > 0){
				VANMANAGER_handleTable[id].flushTimerCnt--;
				if(VANMANAGER_handleTable[id].flushTimerCnt == 0){
					VANMANAGER_handleTable[id].flushTimerCnt = VANMANAGER_FLUSH_TIMER_PERIOD_MS;
					VANMANAGER_handleTable[id].volumeFlushed += VANMANAGER_calVolumeFlushedInTime(id, VANMANAGER_FLUSH_TIMER_PERIOD_MS);
				}
			}
		}
#endif

		// Timeout when cannot reach to volume requested
		if(VANMANAGER_handleTable[id].openVanTimeCnt > 0){
			VANMANAGER_handleTable[id].openVanTimeCnt --;
			if(VANMANAGER_handleTable[id].openVanTimeCnt == 0){
				VANMANAGER_handleTable[id].openVanTimeoutFlag = true;
			}
		}
	}

}
