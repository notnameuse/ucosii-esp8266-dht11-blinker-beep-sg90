#include "led.h"
#include "usart.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
//void led_Init(void)
//{
// 
// GPIO_InitTypeDef  GPIO_InitStructure;
// 	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
//	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PB.5 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
// GPIO_SetBits(GPIOC,GPIO_Pin_13);						 //PB.5 �����

//// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 �˿�����, �������
//// GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
//// GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 ����� 
//}
 
void LED_Init(led_d *led,gpioled port,uint16_t pin)
{
	if(port==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	else if(port==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	else if(port==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	else if(port==GPIOD) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	else if(port==GPIOE) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	else if(port==GPIOF) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	else if(port==GPIOG) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

	
	led->port=port;
	led->pin=pin;
	
	led_config(led);
}


void led_config(led_d *led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = led->pin;				 //LED0-->PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(led->port, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	led_off(led);
}

//�ص�
void led_off(led_d *led)
{
	GPIO_SetBits(led->port,led->pin);
}

//����
void led_on(led_d *led)
{
	GPIO_ResetBits(led->port,led->pin);
}
