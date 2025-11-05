/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"

int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){};
    USART1->DR = (uint8_t) ch;      
	return ch;
}

uint8_t ucUart1RxBuff[1] = {0};
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	HAL_UART_Receive_IT(&huart1, ucUart1RxBuff, 1);
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint8_t uart1CommandBuf[USART1_COMMAND_LEN] = {0}; 	
uint8_t UART1RXSTATE = 0;						// 数据接收完成
uint8_t u8Uart1HeadFlag = 0; 				// 是否接收到帧头
uint16_t u16Uart1RxInex = 0;
uint16_t u16CommandLen = 0;


void reset_uart1_state(void)
{
	u16Uart1RxInex = 0;
	UART1RXSTATE = 0;
	u16CommandLen = 0;
	u8Uart1HeadFlag = 0; // 重置 
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		uint8_t u8ReceivedByte = ucUart1RxBuff[0];
		if(UART1RXSTATE == 0)
		{
			switch (u8Uart1HeadFlag)
			{
				case 0:
					if(u8ReceivedByte  == 0xA0)
					{
						u8Uart1HeadFlag = 1;
						uart1CommandBuf[u16Uart1RxInex] = u8ReceivedByte;
						u16Uart1RxInex++;
					}
					break;
				case 1: // 等待0xBB  
					if (u8ReceivedByte == 0xCC)
					{ 
						u8Uart1HeadFlag = 2;  
						uart1CommandBuf[u16Uart1RxInex] = u8ReceivedByte;
						u16Uart1RxInex++;
					} else {
						reset_uart1_state();
					}  
					break;  
				case 2: // 读取长度字节  
					u16CommandLen = (u8ReceivedByte<<8);  
					if(u16CommandLen > USART1_COMMAND_LEN)
					{  
						reset_uart1_state();
					}else{  
						uart1CommandBuf[u16Uart1RxInex] = u8ReceivedByte;
						u16Uart1RxInex++;
						u8Uart1HeadFlag = 3; 
					}  
					break; 
				case 3:
					u16CommandLen |= u8ReceivedByte;
					if(u16CommandLen > USART1_COMMAND_LEN)
					{  
						reset_uart1_state();
					}else{
						uart1CommandBuf[u16Uart1RxInex] = u8ReceivedByte;
						u16Uart1RxInex++; // 准备接收数据  
						u8Uart1HeadFlag = 4; 
					}
					break;
				case 4: 
					if(u16Uart1RxInex < u16CommandLen) 
					{  
						uart1CommandBuf[u16Uart1RxInex] = u8ReceivedByte; 			
						u16Uart1RxInex++;
						if(u16Uart1RxInex >= u16CommandLen)
						{
							// 数据接收完成  
							UART1RXSTATE = 1;
							u16Uart1RxInex = 0;
							u16CommandLen = 0;
							u8Uart1HeadFlag = 0;
						}
					}  
					break;  
			}
		}
		
		HAL_UART_Receive_IT(&huart1, ucUart1RxBuff, UART1_RX_SIZE);
	}
}

/* USER CODE END 1 */











