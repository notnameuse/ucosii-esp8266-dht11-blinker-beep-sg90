#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
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
//#define LED PCout(13)// PB5

typedef GPIO_TypeDef*   gpioled;

typedef struct{
		gpioled port;
		uint16_t pin;
}led_d;

void LED_Init(led_d *led,gpioled port,uint16_t pin);//��ʼ��
void led_config(led_d *led);
void led_off(led_d *led);
void led_on(led_d *led);
#endif
