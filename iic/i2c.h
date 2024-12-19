#ifndef __I2C_H__
#define __I2C_H__
#include "sys.h"

//ʱ����(SCL)��PB6���---�ߵ�ƽ
#define SCL_H  		GPIO_SetBits(GPIOB,GPIO_Pin_6)  
//ʱ����(SCL)��PB6���---�͵�ƽ											
#define SCL_L  		GPIO_ResetBits(GPIOB,GPIO_Pin_6)

//������(SDA)��PB7����
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
//��ȡPB7�ĵ�ƽ
#define READ_SDA  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
//������(SDA)��PB7���
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//������(SDA)��PB7���---�ߵ�ƽ
#define SDA_H  	GPIO_SetBits(GPIOB,GPIO_Pin_7)	
//������(SDA)��PB7���---�͵�ƽ
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








