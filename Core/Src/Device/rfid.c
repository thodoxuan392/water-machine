/*
 * rfid.c
 *
 *  Created on: Dec 17, 2023
 *      Author: xuanthodo
 */


#include <Device/rfid.h>

#include <PN532/PN532Debug.h>
#include <PN532/PN532.h>

#include <Hal/spi.h>

typedef struct {
	PN532_Handle pn532_handle;
	RFID_Error_t error;
	bool isPlaced;
	bool isDetected;
	RFID_t rfid;
}RFID_Handle;

#define RFID_PN532_DATA_BLOCK	4
static void SPI_CS1_write(const uint8_t *data, const uint32_t data_len);
static void SPI_CS1_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
static void SPI_CS2_write(const uint8_t *data, const uint32_t data_len);
static void SPI_CS2_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
static void SPI_CS3_write(const uint8_t *data, const uint32_t data_len);
static void SPI_CS3_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);

static void RFID_runById(RFID_Id_t id);
static void RFID_buildToBlockData(RFID_t *rfid, uint8_t *data, uint32_t data_len);
static bool RFID_parseFromBlockData(uint8_t *data, uint32_t data_len, RFID_t *rfid);
static uint8_t RFID_getIdKey(uint8_t *id, uint32_t id_len);
static uint16_t RFID_getBlockDataKey(uint8_t *data, uint32_t data_size);


static const uint8_t RFID_key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static RFID_Handle RFID_handleTable[] = {
	[RFID_ID_1] = {
			.pn532_handle = {
				.interface = {
							.write = SPI_CS1_write,
							.write_then_read = SPI_CS1_writeThenRead
				},
			},
	},
	[RFID_ID_2] = {
			.pn532_handle = {
				.interface = {
							.write = SPI_CS2_write,
							.write_then_read = SPI_CS2_writeThenRead
				},
			},
	},
	[RFID_ID_3] = {
			.pn532_handle = {
				.interface = {
							.write = SPI_CS3_write,
							.write_then_read = SPI_CS3_writeThenRead
				},
			},
	}
};

void RFID_init(void){
	for (uint8_t i = 0; i < RFID_ID_MAX; i++)
	{
		PN532_begin(&RFID_handleTable[i].pn532_handle);
	}
}

void RFID_run(void){
	RFID_runById(RFID_ID_1);
	RFID_runById(RFID_ID_2);
	RFID_runById(RFID_ID_3);
}


void RFID_get(RFID_Id_t id, RFID_t * rfid){
	memcpy(rfid, &RFID_handleTable[id].rfid, sizeof(RFID_t));
}

bool RFID_isPlaced(RFID_Id_t id){
	return RFID_handleTable[id].isPlaced;
}

bool RFID_isDetected(RFID_Id_t id){
	return RFID_handleTable[id].isDetected;
}

void RFID_clearDetected(RFID_Id_t id){
	RFID_handleTable[id].isDetected = false;
}

RFID_Error_t RFID_set(RFID_Id_t id, RFID_t * rfid){
//	// Check RFID is available
//	if(!RFID_handleTable[id].isPlaced){
//		return RFID_ERROR_NOT_AVAILABLE;
//	}
//	// Check RFID uid is matched
//	if(rfid->id_len != RFID_handleTable[id].rfid.id_len){
//		return RFID_ERROR_INVALID_FORMAT;
//	}
//	if(memcmp(rfid->id, RFID_handleTable[id].rfid.id, rfid->id_len) != 0){
//		return RFID_ERROR_ID_NOT_MATCHED;
//	}
	uint8_t writeData[16] = {0};
	RFID_buildToBlockData(rfid, writeData, sizeof(writeData));
	if(!PN532_readPassiveTargetID(&RFID_handleTable[id], PN532_MIFARE_ISO14443A, rfid->id, &rfid->id_len, 50)){
		return;
	}
	if(!PN532_mifareclassic_AuthenticateBlock(&RFID_handleTable[id], rfid->id, rfid->id_len, RFID_PN532_DATA_BLOCK, 0, RFID_key)){
		return RFID_ERROR_AUTHEN_FAILED;
	}
	if(!PN532_mifareclassic_WriteDataBlock(&RFID_handleTable[id], RFID_PN532_DATA_BLOCK , writeData)){
		return RFID_ERROR_CANNOT_WRITE;
	}
	return RFID_SUCCESS;
}

bool RFID_isError(RFID_Id_t id){
	return RFID_handleTable[id].error != RFID_SUCCESS;
}

RFID_Error_t RFID_getError(RFID_Id_t id){
	return RFID_handleTable[id].error;
}

void RFID_test(void){
	RFID_t rfid = {
		.id = {163, 52, 18, 8},
		.id_len = 4,
		.money = 200,
		.issueDate = { 24, 1, 7},
		.expireDate = { 25, 1, 7},
	};
	while(1){
		HAL_Delay(1000);
		RFID_set(RFID_ID_1, &rfid);
	}
}

static void RFID_runById(RFID_Id_t id){
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
	uint8_t readBlockData[16] = {0};
	if(!PN532_readPassiveTargetID(&RFID_handleTable[id], PN532_MIFARE_ISO14443A, uid, &uidLength, 50)){
		RFID_handleTable[id].isPlaced = false;
		return;
	}
	if(!PN532_mifareclassic_AuthenticateBlock(&RFID_handleTable[id], uid, uidLength, RFID_PN532_DATA_BLOCK, 0, RFID_key)){
		// Cannot authen block
		return;
	}
	if(!PN532_mifareclassic_ReadDataBlock(&RFID_handleTable[id], RFID_PN532_DATA_BLOCK, readBlockData)){
		// Cannot read data block
		return;
	}

	// Assign uuid to RFID
	RFID_t rfid;
	rfid.id_len = uidLength;
	memcpy(rfid.id, uid, uidLength);

	// Parse RFID
	if(!RFID_parseFromBlockData(readBlockData, sizeof(readBlockData), &rfid)){
		RFID_handleTable[id].error = RFID_ERROR_INVALID_FORMAT;
		return;
	}
	if(!RFID_handleTable[id].isPlaced){
		RFID_handleTable[id].isDetected = true;
	}
	RFID_handleTable[id].isPlaced = true;
	RFID_handleTable[id].error = RFID_SUCCESS;
	RFID_handleTable[id].rfid = rfid;
}

static void SPI_CS1_write(const uint8_t *data, const uint32_t data_len){
	SPI_transmit(SPI_CS_1, data, data_len, 1000);
}
static void SPI_CS1_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
	SPI_transmitAndReceive(SPI_CS_1, tx_buf, tx_len, rx_buf, rx_len, 1000);
}
static void SPI_CS2_write(const uint8_t *data, const uint32_t data_len){
	SPI_transmit(SPI_CS_2, data, data_len, 1000);
}
static void SPI_CS2_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
	SPI_transmitAndReceive(SPI_CS_2, tx_buf, tx_len, rx_buf, rx_len, 1000);
}
static void SPI_CS3_write(const uint8_t *data, const uint32_t data_len){
	SPI_transmit(SPI_CS_3, data, data_len, 1000);
}
static void SPI_CS3_writeThenRead(uint8_t* tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
	SPI_transmitAndReceive(SPI_CS_3, tx_buf, tx_len, rx_buf, rx_len, 1000);
}

static void RFID_buildToBlockData(RFID_t *rfid, uint8_t *data, uint32_t data_len){
	if(data_len != 16){
		return;
	}
	uint8_t data_len_out = 0;
	data[data_len_out++] = (rfid->money >> 8 & 0xFF);
	data[data_len_out++] = rfid->money & 0xFF;
	data[data_len_out++] = rfid->issueDate[0];
	data[data_len_out++] = rfid->issueDate[1];
	data[data_len_out++] = rfid->issueDate[2];
	data[data_len_out++] = rfid->expireDate[0];
	data[data_len_out++] = rfid->expireDate[1];
	data[data_len_out++] = rfid->expireDate[2];

	uint16_t dataKey = RFID_getBlockDataKey(data, data_len_out);
	data[data_len_out++] = (dataKey >> 8 & 0xFF);
	data[data_len_out++] = dataKey & 0xFF;

	uint8_t idKey = RFID_getIdKey(rfid->id, rfid->id_len);
	data[data_len_out++] = idKey;
}

static bool RFID_parseFromBlockData(uint8_t *data, uint32_t data_len, RFID_t *rfid){
	if(data_len != 16){
		return false;
	}
	rfid->money = (uint16_t)data[0] << 8  | data[1];
	rfid->issueDate[0] = data[2];
	rfid->issueDate[1] = data[3];
	rfid->issueDate[2] = data[4];
	rfid->expireDate[0] = data[5];
	rfid->expireDate[1] = data[6];
	rfid->expireDate[2] = data[7];


	uint16_t dataKey = RFID_getBlockDataKey(data, 8);
	uint16_t expectedDataKey = ((uint16_t)data[8] << 8) | data[9];
	if(dataKey != expectedDataKey){
		return false;
	}

	uint8_t idKey = data[10];
	uint8_t expectedIdKey = RFID_getIdKey(rfid->id, rfid->id_len);

	if(idKey != expectedIdKey){
		return false;
	}
	return true;
}

static uint16_t RFID_getBlockDataKey(uint8_t *data, uint32_t data_size){
	uint8_t xor = 0xFF;
	uint8_t sum = 0;
	for (int i = 0; i < data_size; ++i) {
		xor ^= data[i];
		sum += data[i];
	}
	uint16_t ret = ((uint16_t)xor << 8) | sum;
	return ret;
}

static uint8_t RFID_getIdKey(uint8_t *id, uint32_t id_len){
	uint8_t id_key = 0xFF;
	for (int i = 0; i < id_len; ++i) {
		id_key ^= id[i];
	}
	return id_key;
}


