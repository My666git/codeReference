#include "IIC.h"
#include "stm8s_i2c.h"
#include "sys.h"

void I2CInit(void)
{
  I2C_DeInit();  
  I2C_Init(I2C_SPEED, 0x01, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
  I2C_Cmd(ENABLE);  
}

void I2C_WriteByte(uint8_t RegAddr,uint8_t RegData)  
{  
  while(I2C_GetFlagStatus( I2C_FLAG_BUSBUSY));
  I2C_GenerateSTART(ENABLE);
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress( SLAVE_ADDR_7BIT, I2C_DIRECTION_TX );
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  
  I2C_SendData(RegAddr); 
  while (!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  I2C_SendData(RegData);     
  while (!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  I2C_AcknowledgeConfig(I2C_ACK_CURR);  
  I2C_GenerateSTOP(ENABLE);    
}

uint8_t I2C_ReadByte(uint8_t addr)  
{  
  uint8_t i;  
  while(I2C_GetFlagStatus( I2C_FLAG_BUSBUSY));  
  I2C_GenerateSTART( ENABLE);
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress( SLAVE_ADDR_7BIT, I2C_DIRECTION_TX );
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));  
  I2C_SendData( addr);
  while (!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  I2C_GenerateSTART( ENABLE);
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress( SLAVE_ADDR_7BIT, I2C_DIRECTION_RX );
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));  
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));  
  i=I2C_ReceiveData();
  I2C_AcknowledgeConfig(I2C_ACK_NONE);  
  I2C_GenerateSTOP( ENABLE);
  return i;
} 


void I2C_WriteMultiByte(uint8_t RegAddr,uint8_t *RegData,uint8_t DataCount)
{
  uint8_t i = 0;
  while(I2C_GetFlagStatus( I2C_FLAG_BUSBUSY));
  I2C_GenerateSTART(ENABLE);//    I2C1  
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT));/*EV5,  ??*/  
  I2C_Send7bitAddress( SLAVE_ADDR_7BIT, I2C_DIRECTION_TX );//      ? 
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  
  I2C_SendData(RegAddr);// ?     ?  
  while (!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  for(i=0;i<DataCount;i++)
  {
    I2C_SendData(RegData[i]);//          
    while (!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
    I2C_AcknowledgeConfig(I2C_ACK_CURR);
  }
  I2C_GenerateSTOP( ENABLE);// ? I2C1      
  
}




void IIC_Init(void)
{
        
     GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);  //  ʼ  IO   ģʽ   PB4-SCL   Ӧ  IO ӿڳ ʼ  
     GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);  //  ʼ  IO   ģʽ   PB5-SDA   Ӧ  IO ӿڳ ʼ  
    
}

void IIC_Start(void)
{
	SDA_OUT();     //sda     
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(4);
 	IIC_SDA_L;    //START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_L;    //ǯסI2C   ߣ ׼     ͻ         
}


void IIC_Stop(void)
{
	SDA_OUT();//sda     
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_H; 
	IIC_SDA_H;//    I2C   ߽    ź 
	delay_us(4);							   	
}



u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
        
	SDA_IN();      //SDA    Ϊ      
	IIC_SDA_H;
        delay_us(2);	   
	IIC_SCL_H;
        delay_us(2);	 
        
	while(READ_IIC_SDA)
	{
	   ucErrTime++;
	   if(ucErrTime>250)
	     {
               IIC_Stop();
               return 1;
             }
	}
	IIC_SCL_L;//ʱ     0 	   
	return 0;  
} 


void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
}


void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	delay_us(2);
	IIC_SCL_H;
	delay_us(2);
	IIC_SCL_L;
}

void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
   
    SDA_OUT(); 
    
    IIC_SCL_L;//    ʱ ӿ ʼ   ݴ   
    for(t=0;t<8;t++)
    {              
       
	if((txd&0x80)>>7)
		IIC_SDA_H;
		else
		IIC_SDA_L;
                
		txd<<=1; 	  
		delay_us(5); 
		IIC_SCL_H;
		delay_us(5); 
		IIC_SCL_L;	
		delay_us(5);
    }	 
} 



u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	SDA_IN();
	
        for(i=0;i<8;i++ )
	{
        IIC_SCL_L; 
        delay_us(2);
	IIC_SCL_H;
        receive<<=1;
        
        if(READ_IIC_SDA)receive++;  
        
		delay_us(2); 
    }					 
    if (!ack)
        IIC_NAck();//    nACK
    else
        IIC_Ack(); //    ACK   
    return receive;
}








