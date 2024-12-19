/*
 * mbrtu.c
 *
 *  Created on: Oct 9, 2022
 *      Author: A
 */

#include "mbrtu.h"
#include "mb_crc16.h"
#include "usart.h"
#include <limits.h>

/*
 * ModBus-Rtu发送接收指令数组
 */
uint8_t mbrtu_send[MBRTU_SIZE_MAX] = {0x00};
uint8_t mbrtu_recv[MBRTU_SIZE_MAX] = {0x00};

/*
 * ModBus-Rtu指令发送
 */
bool MBRTUSend(const MB mb, const uint8_t data[]) {

	U2_RS485_TX_EN;

	uint8_t pos = 0;
	uint16_t temp = 0;

	mbrtu_send[pos++] = mb.mbId;
	mbrtu_send[pos++] = mb.funCode;
	mbrtu_send[pos++] = (mb.regAddr >> CHAR_BIT) & 0xFF;
	mbrtu_send[pos++] = mb.regAddr & 0xFF;
	mbrtu_send[pos++] = (mb.len >> CHAR_BIT) & 0xFF;
	mbrtu_send[pos++] = mb.len & 0xFF;

	if (mb.funCode == WRITEMULREG) {
		mbrtu_send[pos++] = mb.byteCount;
		for (uint8_t i = 0; i < mb.byteCount; i++) {
			mbrtu_send[pos++] = data[i];
		}
	}

	/* CRC */
	temp = usMBCRC16(mbrtu_send, pos);
	mbrtu_send[pos++] = temp & 0xFF;
	mbrtu_send[pos++] = (temp >> CHAR_BIT) & 0xFF;

	if (HAL_UART_Transmit(&huart2, mbrtu_send, pos, TIMEOUT_100MS) != HAL_OK) {
		return false;
	}

	return true;
}

/*
 * ModBus-Rtu指令完成接收
 */
bool MBRTURecv(const MB mb) {

	U2_RS485_RX_EN;

	uint16_t temp = 0;
	uint16_t len = 0;

	if (mb.funCode == READHOLDREG) {
		/* mb.len: 16bit data length -> 8bit data length * 2 */
		len = mb.len * 2;
		if (HAL_UART_Receive(&huart2, mbrtu_recv, MBRTU_DATA_BEGIN + len + MBRTU_SIZE_CRC, TIMEOUT_100MS) != HAL_OK) {
			return false;
		}

		/* CRC */
		temp = (((uint16_t)mbrtu_recv[MBRTU_DATA_BEGIN + len + 1] & 0x00FF) << CHAR_BIT) | ((uint16_t)mbrtu_recv[MBRTU_DATA_BEGIN + len] & 0x00FF);
		if (temp != usMBCRC16(mbrtu_recv, MBRTU_DATA_BEGIN + len)) {
			return false;
		}
	}

	else if (mb.funCode == WRITEMULREG) {
		len = MBRTU_STD_LEN;
		if (HAL_UART_Receive(&huart2, mbrtu_recv, len, TIMEOUT_100MS) != HAL_OK) {
			return false;
		}

		/* CRC */
		temp = (((uint16_t)mbrtu_recv[MBRTU_STD_LEN - MBRTU_SIZE_CRC + 1] & 0x00FF) << CHAR_BIT) | ((uint16_t)mbrtu_recv[MBRTU_STD_LEN - MBRTU_SIZE_CRC] & 0x00FF);
		if (temp != usMBCRC16(mbrtu_recv, MBRTU_STD_LEN - MBRTU_SIZE_CRC)) {
			return false;
		}
	}

	return true;
}

/*
 * ModBus-Rtu指令执行
 */
bool MBRTUExecute(const MB mb, const uint8_t data[]) {
	/* 无限等待 */
	do {
		MBRTUSend(mb, data);
	} while(!MBRTURecv(mb));

	return true;
}
