#include "IIC.h"


static void delay_us(__IO uint32_t t)
{

	while(t--);

}


void IIC_Init(void)
{			
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		
	GPIO_InitStruct.Pin   = GPIO_PIN_2;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	IIC_SCL_H;
	IIC_SDA_H;
}

void SDA_OUT(void)
{			
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_2;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void SDA_IN(void)
{			
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin   = GPIO_PIN_2;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}





//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_us(500);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_us(500);
	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_us(500);
	IIC_SCL_H; 
	IIC_SDA_H;//����I2C���߽����ź�
	delay_us(500);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_H;delay_us(200);	   
	IIC_SCL_H;delay_us(200);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	delay_us(200);
	IIC_SCL_H;
	delay_us(200);
	IIC_SCL_L;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	delay_us(200);
	IIC_SCL_H;
	delay_us(200);
	IIC_SCL_L;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
		SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
				if((txd&0x80)>>7)IIC_SDA_H;
				else IIC_SDA_L;
        txd<<=1; 	  
				delay_us(200);   //��TEA5767��������ʱ���Ǳ����
				IIC_SCL_H;
				delay_us(200); 
				IIC_SCL_L;	
				delay_us(200);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
        IIC_SCL_L; 
        delay_us(200);
				IIC_SCL_H	;
        receive<<=1;
        if(READ_SDA)receive++;   
				delay_us(200); 
   }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

