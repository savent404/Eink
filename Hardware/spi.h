#ifndef __SPI_H
#define __SPI_H

#include "stm32f2xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

 
void MX_SPI1_Init(void);
extern SPI_HandleTypeDef hspi1;
uint8_t SPIx_ReadWriteByte(uint8_t TxData);
//void SPIx_Init(void);			 //��ʼ��SPI��
//void SPIx_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
//u8 SPIx_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
//		 
#endif

