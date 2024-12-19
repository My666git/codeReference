/*
 * mbrtu.h
 *
 *  Created on: Oct 9, 2022
 *      Author: A
 */
#include "main.h"

#ifndef INC_MBRTU_H_
#define INC_MBRTU_H_

#define READHOLDREG			3
#define WRITEMULREG 		16

#define MBRTU_STD_LEN		8
#define MBRTU_SIZE_MAX		256
#define MBRTU_SIZE_MIN		4
#define MBRTU_SIZE_CRC		2

#define MBRTU_DATA_BEGIN	3
/*
 * MBInfo
 */
typedef struct {
	uint8_t mbId;
	uint8_t funCode;
	uint16_t regAddr;
	uint16_t len;
	uint8_t byteCount;
}MB, *MB_p;

extern uint8_t mbrtu_send[MBRTU_SIZE_MAX];
extern uint8_t mbrtu_recv[MBRTU_SIZE_MAX];

bool MBRTUExecute(const MB mb, const uint8_t data[]);
bool MBRTUSend(const MB mb, const uint8_t data[]);
bool MBRTURecv(const MB mb);

#endif /* INC_MBRTU_H_ */
