#ifndef __I2C_H__
#define __I2C_H__
#include "sys.h"

//时钟线(SCL)：PB6输出---高电平
#define SCL_H  		GPIO_SetBits(GPIOB,GPIO_Pin_6)  
//时钟线(SCL)：PB6输出---低电平											
#define SCL_L  		GPIO_ResetBits(GPIOB,GPIO_Pin_6)

//数据线(SDA)：PB7输入
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
//读取PB7的电平
#define READ_SDA  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
//数据线(SDA)：PB7输出
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//数据线(SDA)：PB7输出---高电平
#define SDA_H  	GPIO_SetBits(GPIOB,GPIO_Pin_7)	
//数据线(SDA)：PB7输出---低电平
#define SDA_L  	GPIO_ResetBits(GPIOB,GPIO_Pin_7)


#define DAC121C085_ADDR_R			(0x13)
#define DAC121C085_ADDR_W			(0x12)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
u8 IIC_Wait_Ack(void);
void IIC_Send_Byte(u8 txdata);
u8 IIC_Read_Byte(unsigned char ack);
void WriteDACRegister(u8 data_h, u8 data_l);
uint8_t reversal(uint8_t base);
uint16_t ReadDACRegister(void);

#endif








