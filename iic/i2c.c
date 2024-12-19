#include "i2c.h"
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"


//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	//PB6,PB7 ����ߵ�ƽ---����״̬
}

void IIC_Start(void)//��ʼ�ź�
{
	SDA_OUT();			//������(SDA)���ģʽ
	SDA_H;				//������(SDA)����ߵ�ƽ
	SCL_H;				//ʱ����(SCL)����ߵ�ƽ
	delay_us(5);
	SDA_L;				//������(SDA)����͵�ƽ
	delay_us(5);
	SCL_L;				//ʱ����(SCL)����͵�ƽ
}


void IIC_Stop(void)//��ֹ�ź�
{
	SDA_OUT();		//������(SDA)���ģʽ
	SCL_L;			//ʱ����(SCL)����͵�ƽ
	SDA_L;			//������(SDA)����͵�ƽ
	delay_us(5);
	SCL_H;			//ʱ����(SCL)����ߵ�ƽ
	SDA_H;			//������(SDA)����ߵ�ƽ
	delay_us(5);
}


void IIC_Ack(void)//����Ӧ���ź�
{
	SCL_L;			//ʱ����(SCL)����͵�ƽ
	SDA_OUT();		//������(SDA)���ģʽ
	SDA_L;			//������(SDA)����͵�ƽ
	delay_us(2);
	SCL_H;			//ʱ����(SCL)����ߵ�ƽ
	delay_us(5);
	SCL_L;			//ʱ����(SCL)����͵�ƽ
}


void IIC_NAck(void)//������Ӧ���ź�
{
	SCL_L;			//ʱ����(SCL)����͵�ƽ
	SDA_OUT();		//������(SDA)���ģʽ
	SDA_H;			//������(SDA)����ߵ�ƽ
	delay_us(2);
	SCL_H;			//ʱ����(SCL)����ߵ�ƽ
	delay_us(5);
	SCL_L;			//ʱ����(SCL)����͵�ƽ
}


//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)//�ȴ�Ӧ���ź�
{
	int uc=0;
	SDA_IN();		//������(SDA)����ģʽ
	SDA_H;  delay_us(1);
	SCL_H;  delay_us(1);
	while(READ_SDA)
	{
		uc++;
		if(uc>250)//����Ӧ��ʧ��
		{
			IIC_Stop();//������ֹ�ź�		
			return 1;
		}
	}
	SCL_L;
	return 0;
}


void IIC_Send_Byte(u8 txdata)//����һ���ֽ�����---��λ��ǰ����λ�ں�
{
	SDA_OUT();				//������(SDA)���ģʽ
	SCL_L;
	for(int t=0;t<8;t++)	//��λ��ǰ����λ�ں�
	{
		if((txdata&0x80)>>7)   	SDA_H;
		else                	SDA_L;
		txdata<<=1;
		delay_us(2);
		SCL_H;
		delay_us(2);
		SCL_L;
		delay_us(2);
	}
}


u8 IIC_Read_Byte(unsigned char ack)//��ȡһ���ֽ�����---ack=1ʱ������ACK��ack=0������nACK
{
	unsigned char rxdata=0,i;
	SDA_IN();		//������(SDA)����ģʽ
	for(i=0;i<8;i++)
	{
		SCL_L;
		delay_us(2);
		SCL_H;
		rxdata<<=1;
		if(READ_SDA)
			rxdata++;
		delay_us(1);
	}
	if(!ack)   IIC_NAck();
	else       IIC_Ack();
	return rxdata;
}


void WriteDACRegister(u8 data_h, u8 data_l)
{
	IIC_Start();
	IIC_Send_Byte((u8)DAC121C085_ADDR_W);
	if(IIC_Wait_Ack() == 0)
	{
		IIC_Send_Byte(data_h);
		if(IIC_Wait_Ack() != 0)
		{
			printf("ACK1 error...");
		}
		IIC_Send_Byte(data_l);
		if(IIC_Wait_Ack() != 0)
		{
			printf("ACK2 error...");
		}
		IIC_Stop();
	}
	else
	{
		printf("IIC write address error...");
	}
}


uint16_t ReadDACRegister(void)
{
	uint8_t data_h = 0;
	uint8_t data_l = 0;
	IIC_Start();
	IIC_Send_Byte((u8)DAC121C085_ADDR_R);
	if(IIC_Wait_Ack() != 0)
	{
		printf("ACK1 error...");
	}
	data_h = IIC_Read_Byte(1);
	data_l = IIC_Read_Byte(0);
	IIC_Stop();
	
	data_h = reversal(data_h);
	data_l = reversal(data_l);
	return (data_h<<8)|data_l;
}


uint8_t reversal(uint8_t base)
{
	uint8_t res = 0;
	uint8_t i = 0;
	for(i=0; i<8; i++)
	{
		res = res << 1;
		res += (base>>i) & 1;
	}
	return res;
}


