#include "Eink.h"
#include "flash.h"
#include "TPS65185.h"

typedef enum {FALSE = 0, TRUE = !FALSE} bool;


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//��������

//��ͼƬ��ˢ����
#if 0
#define FRAME_BEGIN_LEN		18
const unsigned char wave_begin[4][FRAME_BEGIN_LEN]=
{
0,0,0,0,0,2,2,2,2,1,1,1,1,2,2,2,2,0,						//GC0->GC3
0,0,0,1,1,2,2,2,2,1,1,1,1,2,2,2,2,0,						//GC1->GC3
0,0,1,1,1,2,2,2,2,1,1,1,1,2,2,2,2,0,						//GC2->GC3
0,1,1,1,1,2,2,2,2,1,1,1,1,2,2,2,2,0,						//GC3->GC3
};
#else
#define FRAME_BEGIN_LEN		10
const unsigned char wave_begin[4][FRAME_BEGIN_LEN]=
{
0,0,0,0,1,2,2,2,2,0,						//GC0->GC3
0,0,0,1,1,2,2,2,2,0,						//GC1->GC3
0,0,1,1,1,2,2,2,2,0,						//GC2->GC3
0,1,1,1,1,2,2,2,2,0,						//GC3->GC3
};
#endif

//�Ӱ�ˢ����ͼƬ
#define FRAME_END_LEN		15
const unsigned char wave_end[4][FRAME_END_LEN]=
{
0,1,1,1,1,2,2,2,2,1,1,1,1,0,0,				//GC3->GC0
0,1,1,1,1,2,2,2,2,1,1,0,0,0,0,				//GC3->GC1
0,1,1,1,1,2,2,2,2,1,0,0,0,0,0,				//GC3->GC2
0,1,1,1,1,2,2,2,2,0,0,0,0,0,0,				//GC3->GC3
};

#if 1
//�ڰ׺ڰ�ˢ�������յ���ױ���
#define FRAME_INIT_LEN 		32
const unsigned char wave_init[FRAME_INIT_LEN]=
{
	
//	0xaa,0xaa,0xaa,0xaa,
//	
	0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,
	0x55,0x55,0x55,0x55,	
  0xaa,0xaa,0xaa,0xaa,
	0xaa,0xaa,0xaa,0xaa,
	0xaa,0xaa,0xaa,0xaa,
	//0x55,0x55,0x55,0x55,
	0xaa,0xaa,0xaa,0xaa,
	0xaa,0xaa,0xaa,0xaa,

};
#endif

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//ȫ�ֱ���
//������Ram�Ĳ������ݱ�
unsigned char wave_begin_table[256][FRAME_BEGIN_LEN]={0};
unsigned char wave_end_table[256][FRAME_END_LEN]={0};

unsigned char g_dest_data[200];		//�͵�����ֽ��һ�����ݻ���
unsigned char loop = 0;						//ѭ������
unsigned char column = 0;					//����ֽ�������� ����
unsigned int old_data;						//����ֽ�������� ����
unsigned int new_data;						//����ֽ�������� ����

							
GPIO_InitTypeDef GPIO_InitStructure;
//ErrorStatus HSEStartUpStatus;

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//��������
void Delay(__IO uint32_t nCount);

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void line_data_init(uint8_t frame)
{
	int i;
	
	for(i=0; i<200; i++)
	{
		g_dest_data[i] = wave_init[frame];	
	}	
}

void line_begin_pic(uint8_t *old_pic, uint8_t frame)
{
	int i;
	
	for(i=0; i<200; i++)
	{
		g_dest_data[i] = wave_begin_table[old_pic[i]][frame];	
	}	
}

void line_end_pic(uint8_t *new_pic, uint8_t frame)
{
	int i;
	
	for(i=0; i<200; i++)
	{
		g_dest_data[i] = wave_end_table[new_pic[i]][frame];	
	}	
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//ͼƬ������ʾ
void EPD_Display_PIC(unsigned char *ptr)
{
  int line, frame;
	EPD_PowerOn(0x1DF);


#if 0
	//�ȶ���ͼƬ��ˢ����

	for(frame=0; frame<FRAME_BEGIN_LEN; frame++)			
	{
		EPD_Start_Scan();
		for(line=0; line<600; line++)
		{
			line_begin_pic(ptr + line*200, frame);			//42ms
			EPD_Send_Row_Data( g_dest_data );				//40ms
		}
		EPD_Send_Row_Data( g_dest_data );					//���һ�л���GATE CLK,���ٴ�һ��û������
	}
#endif

#if 1		
	//�ڰ׺ڰ�ˢ�������յ���ױ���
	EPD_PowerOn(0x000);
	for(frame=0; frame<FRAME_INIT_LEN; frame++)			
	{
		EPD_Start_Scan();
		for(line=0; line<600; line++)
		{
			line_data_init(frame);							//14ms
			line_data_init(frame);							//14ms	�Ӹ�ָ���Ǳ���ʱ����ChangePICʱһ��
			line_data_init(frame);							//14ms	�Ӹ�ָ���Ǳ���ʱ����ChangePICʱһ��
			EPD_Send_Row_Data( g_dest_data );				//40ms
		}
		EPD_Send_Row_Data( g_dest_data );					//���һ�л���GATE CLK,���ٴ�һ��û������
	}
#endif

#if 1	
	//�Ӱ�ˢ����ͼƬ
	EPD_PowerOn(0x070);
	for(frame=0; frame<FRAME_END_LEN; frame++)					
	{
		EPD_Start_Scan();
		for(line=0; line<600; line++)
		{
			//SPI_Flash_Read((uint8_t*)PIC,line*200,200);
			line_end_pic(ptr + line*200, frame);			//42ms

			EPD_Send_Row_Data( g_dest_data );				//40ms 
		}
		EPD_Send_Row_Data( g_dest_data );					//���һ�л���GATE CLK,���ٴ�һ��û������
	}
	
		for(frame=9; frame<FRAME_END_LEN; frame++)					
	{
		EPD_Start_Scan();
		for(line=0; line<600; line++)
		{
			//SPI_Flash_Read((uint8_t*)PIC,line*200,200);
			//line_end_pic(PIC, frame);			//42ms
			line_end_pic(ptr + line*200, frame);			//42ms

			EPD_Send_Row_Data( g_dest_data );				//40ms 
		}
		EPD_Send_Row_Data( g_dest_data );					//���һ�л���GATE CLK,���ٴ�һ��û������
	}
		for(frame=9; frame<FRAME_END_LEN; frame++)					
	{
		EPD_Start_Scan();
		for(line=0; line<600; line++)
		{
			//SPI_Flash_Read((uint8_t*)PIC,line*200,200);
			line_end_pic(ptr + line*200, frame);			//42ms

			EPD_Send_Row_Data( g_dest_data );				//40ms 
		}
		EPD_Send_Row_Data( g_dest_data );					//���һ�л���GATE CLK,���ٴ�һ��û������
	}
	

#endif
		
	EPD_PowerOff();
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void SYS_Init(void)
{
		  GPIO_InitTypeDef GPIO_InitStruct;
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
		__GPIOC_CLK_ENABLE();
	
	
//  	//ʹ��ϵͳʹ�õ���GPIO��ʱ��
//    //����EPDʹ�õ���GPIO�Ĳ��� 
	
		GPIO_InitStruct.Pin = PORT_A_PIN_OUT;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = PORT_B_PIN_OUT;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = PORT_C_PIN_OUT;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}


//��ʱ����
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

//������RAM�еĲ��α����������ɨ���ٶ�
void make_wave_table(void)
{
	int frame, num;
	unsigned char tmp,value;
	

	//wave_begin_table
	for(frame=0; frame<FRAME_BEGIN_LEN; frame++)
	{		
		for(num=0; num<256; num++)
		{
			tmp = 0;
			tmp = wave_begin[(num>>6)&0x3][frame];
					
			tmp = tmp<< 2;
			tmp &= 0xfffc;
			tmp |= wave_begin[(num>>4)&0x3][frame];

			tmp = tmp<< 2;
			tmp &= 0xfffc;
			tmp |= wave_begin[(num>>2)&0x3][frame];

			tmp = tmp<< 2;
			tmp &= 0xfffc;
			tmp |= wave_begin[(num)&0x3][frame];

			value = 0;
			value = (tmp <<6) & 0xc0;
			value += (tmp<<2) & 0x30;
			value += (tmp>>2) & 0x0c;
			value += (tmp>>6) & 0x03;
			wave_begin_table[num][frame] = value;
		}
	}

	//wave_end_table
	for(frame=0; frame<FRAME_END_LEN; frame++)
	{		
		for(num=0; num<256; num++)
		{
			tmp = 0;
			tmp = wave_end[(num>>6)&0x3][frame];
					
			tmp = tmp<< 2;
			tmp &= 0xfffc;
			tmp |= wave_end[(num>>4)&0x3][frame];

			tmp = tmp<< 2;
			tmp &= 0xfffc;
			tmp |= wave_end[(num>>2)&0x3][frame];

			tmp = tmp<< 2;
			tmp &= 0xfffc;
			tmp |= wave_end[(num)&0x3][frame];

			value = 0;
			value = (tmp <<6) & 0xc0;
			value += (tmp<<2) & 0x30;
			value += (tmp>>2) & 0x0c;
			value += (tmp>>6) & 0x03;
			wave_end_table[num][frame] = value;
		}
	}
}





//��ѹ��VCOM��
void EPD_PowerOn(uint16_t Vcom)
{ 

	WAKUP_L;
	//delay_us(1000);   
	WAKUP_H; 
//	PWRUP_H;
	VCOMCTRL_H;
	//delay_us(1000);
	TPS65185_WriteOneByte(VCOM2,(Vcom>>8)&0x01);
	TPS65185_WriteOneByte(VCOM1,Vcom&0xFF);

	TPS65185_WriteOneByte(ENABLE,0xBF);
	

	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//��ѹ��VCOM��  
void EPD_PowerOff(void)
{
	
	TPS65185_WriteOneByte(ENABLE,0x00);
//	HAL_GPIO_WritePin(VCOM_CTR_PORT, VCOM_CTR,GPIO_PIN_RESET);				//VCOM = 0
//	Delay(0xff);
//	
//	HAL_GPIO_WritePin(GVDD22_CTR_PORT, GVDD22_CTR,GPIO_PIN_RESET);
//	Delay(0xff);
//	HAL_GPIO_WritePin(VPOS15_CTR_PORT, VPOS15_CTR,GPIO_PIN_RESET);
//	Delay(0xff);
//	HAL_GPIO_WritePin(VNEGGVEE_CTR_PORT, VNEGGVEE_CTR,GPIO_PIN_RESET);
//	Delay(0xff);
}


//����ֽIO�ڳ�ʼ��
void EPD_Init(void)
{
	HAL_GPIO_WritePin(EPD_SHR_PORT, EPD_SHR,GPIO_PIN_RESET);               	//Shift direction source driver	  
	HAL_GPIO_WritePin(EPD_GMODE1_PORT, EPD_GMODE1,GPIO_PIN_SET);				//one pulse mode	
	HAL_GPIO_WritePin(EPD_GMODE2_PORT, EPD_GMODE2,GPIO_PIN_SET);				//one pulse mode
	HAL_GPIO_WritePin(EPD_XRL_PORT, EPD_XRL,GPIO_PIN_SET);               		//Shift direction gate driver
	//GPIO_ResetBits(EPD_XRL_PORT, EPD_XRL);               		//Shift direction gate driver
//	HAL_GPIO_WritePin(EPD_XCE1_PORT, EPD_XCE1,GPIO_PIN_RESET);       			//EPD����	
//	HAL_GPIO_WritePin(EPD_XCE2_PORT, EPD_XCE2,GPIO_PIN_RESET);       			//EPD����
	


	EPD_PowerOff();

	EPD_LE_L;
	EPD_CL_L;
	EPD_OE_L;
	EPD_SPH_H;
	EPD_XSPV_H;
	EPD_CLK_L;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//EPD��ʼʱ��
void EPD_Start_Scan(void)
{
	   
	EPD_XSPV_H;                                             //XSPV�ȸ�

	loop = 2;                                       		//DCLK��2��ʱ��
	while(loop--)                            
	{
		EPD_CLK_L;
		Delay(0x1f);	
		EPD_CLK_H;
		Delay(0x1f);
	}
 
	EPD_XSPV_L;                                             //XSPV�ٵ�

	loop = 2;
	while(loop--)
	{
		EPD_CLK_L;                    						//DCLK��2��ʱ��
		Delay(0x1f);
		EPD_CLK_H;
		Delay(0x1f);
	}

	EPD_XSPV_H;                                             //XSPV  ���
	loop = 2;
	while(loop--)
	{
		EPD_CLK_L;
		Delay(0x1f);
		EPD_CLK_H;
		Delay(0x1f);
	}
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//��ɨ��
void EPD_Send_Row_Data(uint8_t *pArray )      					//  �����ݣ�
{

	EPD_LE_H;                                              	//LE �ȵ�
	EPD_CL_L;
	EPD_CL_H;
	EPD_CL_L;
	EPD_CL_H;
	
	EPD_LE_L;  												//LE ��
	EPD_CL_L;
	EPD_CL_H;
	EPD_CL_L;
	EPD_CL_H;
	
	EPD_OE_H;                                              	//OE �ȸ�
	EPD_CL_L;
	EPD_CL_H;
	EPD_CL_L;
	EPD_CL_H;

	EPD_SPH_L;                                              //STL �ȵͣ�	                                           
	
	old_data = (uint16_t)(EPD_DB_PORT->ODR);
	old_data &= 0xFF00;
	for(column= 0; column < 200; column++)          		// дһ�����ݣ� 
	{
		new_data = old_data  | (uint16_t)pArray[column];
		(EPD_DB_PORT->ODR) = new_data;
		EPD_CL_L;
		EPD_CL_H;
	}

	EPD_SPH_H;                                             	//STL ��
 
	EPD_CL_L;
	EPD_CL_H;
	EPD_CL_L;
	EPD_CL_H;

  	EPD_CLK_L;                                               //DCLK  ���
	EPD_OE_L;                                                //OE  ���
	EPD_CL_L;
	EPD_CL_H;
	EPD_CL_L;
	EPD_CL_H;
	
	EPD_CLK_H;                                               //DCLK  ���( DCLK һ��ʱ��)
}





void line_data_init_Color(uint8_t Color)
{
	int i;
	
	for(i=0; i<200; i++)
	{
		g_dest_data[i] = Color;	
	}	
}
