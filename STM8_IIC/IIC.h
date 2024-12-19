#ifndef __IIC__H_
#define __IIC__H_

#include "stm8s.h"

#define SLAVE_ADDR_7BIT 0x52
#define I2C_SPEED 400000



#define IIC_SCL_H      GPIO_WriteHigh(GPIOB, GPIO_PIN_4)    //SCL--PB4	输出高电平
#define IIC_SCL_L      GPIO_WriteLow(GPIOB, GPIO_PIN_4)     //SCL--PB4	输出低电平

#define IIC_SDA_H      GPIO_WriteHigh(GPIOB, GPIO_PIN_5)   //SDA--PB5	输出高电平 
#define IIC_SDA_L      GPIO_WriteLow(GPIOB, GPIO_PIN_5)    //SDA--PB5	输出低电平

#define READ_IIC_SDA   GPIO_ReadInputPin(GPIOB, GPIO_PIN_5)  //输入SDA 

#define SDA_OUT()      GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST)  //初始化IO输出模式   PB5-SDA 对应的IO接口初始化  //设置为输出

#define SDA_IN()       GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT)  //初始化IO输出模式  PB5-SDA  对应的IO接口初始化   //设置为输入


//IIC所有操作函数
void IIC_Init(void);                           //初始化IIC的IO口			
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);           //IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 

void I2CInit(void);
void I2C_WriteByte(uint8_t RegAddr,uint8_t RegData);
uint8_t I2C_ReadByte(uint8_t addr);

void I2C_WriteMultiByte(uint8_t RegAddr,uint8_t *RegData,uint8_t DataCount);

#endif