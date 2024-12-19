#ifndef __IIC__H_
#define __IIC__H_

#include "stm8s.h"

#define SLAVE_ADDR_7BIT 0x52
#define I2C_SPEED 400000



#define IIC_SCL_H      GPIO_WriteHigh(GPIOB, GPIO_PIN_4)    //SCL--PB4	����ߵ�ƽ
#define IIC_SCL_L      GPIO_WriteLow(GPIOB, GPIO_PIN_4)     //SCL--PB4	����͵�ƽ

#define IIC_SDA_H      GPIO_WriteHigh(GPIOB, GPIO_PIN_5)   //SDA--PB5	����ߵ�ƽ 
#define IIC_SDA_L      GPIO_WriteLow(GPIOB, GPIO_PIN_5)    //SDA--PB5	����͵�ƽ

#define READ_IIC_SDA   GPIO_ReadInputPin(GPIOB, GPIO_PIN_5)  //����SDA 

#define SDA_OUT()      GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST)  //��ʼ��IO���ģʽ   PB5-SDA ��Ӧ��IO�ӿڳ�ʼ��  //����Ϊ���

#define SDA_IN()       GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT)  //��ʼ��IO���ģʽ  PB5-SDA  ��Ӧ��IO�ӿڳ�ʼ��   //����Ϊ����


//IIC���в�������
void IIC_Init(void);                           //��ʼ��IIC��IO��			
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);           //IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 

void I2CInit(void);
void I2C_WriteByte(uint8_t RegAddr,uint8_t RegData);
uint8_t I2C_ReadByte(uint8_t addr);

void I2C_WriteMultiByte(uint8_t RegAddr,uint8_t *RegData,uint8_t DataCount);

#endif