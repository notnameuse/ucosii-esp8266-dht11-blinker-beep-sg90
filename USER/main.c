#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "includes.h"
#include "adc.h"
#include "exti.h"
#include "lcd.h"
#include "gui.h"
#include "dht11.h"
#include "beep.h"


/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//LED����
//�����������ȼ�
#define LED0_TASK_PRIO       			6
//���������ջ��С
#define LED0_STK_SIZE  		    		512
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);


//���ܴ�����
//�����������ȼ�
#define PWM_TASK_PRIO       			7
//���������ջ��С
#define PWM_STK_SIZE  					64
//�����ջ
OS_STK PWM_TASK_STK[PWM_STK_SIZE];
//������
void pwm_task(void *pdata);

//������������
#define ADC_TASK_PRIO            9
#define ADC_STK_SIZE						512
OS_STK  ADC_TASK_STK[ADC_STK_SIZE];
void 		adc_task(void *pdata);

//dht11����
#define dht11_task_prio 				8
#define dht11_stk_size  			  512
OS_STK dht11_task_stk[dht11_stk_size];
void dht11_task(void *pdata);

//��������������
#define beep_task_prio         5
#define beep_stk_size          64
OS_STK  beep_task_stk[beep_stk_size];
void beep_task(void *pdata);


//OS_EVENT *Bsem; 
OS_EVENT *email;
OS_EVENT *sig1;
OS_EVENT *sig2;
led_d led1;
led_d p14;
led_d bep;
static u8 flag=0;
static u8 mark=0;
static u8 biao=0; 



 int main(void)
 {	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    //��ʱ������ʼ��	
	//SCB->VTOR = FLASH_BASE | 0x4000;	 
	LED_Init(&led1,GPIOC,GPIO_Pin_13);			 //��ʼ����LED���ӵ�Ӳ���ӿ�
	Beep_Init(&bep,GPIOB,GPIO_Pin_15);
	TIM3_PWM_Init(99,14399);
	Adc_Init();
	EXTIX_Init();
	LCD_Init();	
	uart_init(9600);
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	  	 
}
	  
//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	//Bsem=OSSemCreate(0);
	sig1=OSSemCreate(0);
	sig2=OSSemCreate(0);
	email=OSMboxCreate((void *)0);
  OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);	//Ϊʲô����ĵڶ��������ǿ�ָ���أ���Ϊ���������Ҫ��ֵ��R0�Ĵ�����					   
 	OSTaskCreate(pwm_task,(void *)0,(OS_STK*)&PWM_TASK_STK[PWM_STK_SIZE-1],PWM_TASK_PRIO);	//��R0�Ĵ����Ƕ�ջָ��PSP�����PSP=0�����������һ�ν��������л�������
	OSTaskCreate(adc_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_TASK_PRIO);//��������ʱ�������ҪΪ0
	OSTaskCreate(dht11_task,(void *)0,(OS_STK*)&dht11_task_stk[dht11_stk_size-1],dht11_task_prio);
	OSTaskCreate(beep_task,(void *)0,(OS_STK*)&beep_task_stk[beep_stk_size-1],beep_task_prio);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//LED����
void led0_task(void *pdata)
{	
	u8 err; 	
	u16 *x;
	while(1)
	{
		x=(u16 *)OSMboxPend(email,0,&err);
//		Show_Str(20,30,BLUE,WHITE,"Light:      C",16,0);
//		LCD_ShowNum(20+6*8,30,*x,2,16);
//		printf("��ʱ�Ĺ�������ֵΪ��");
//		printf("%d\n",*x);
//		printf("��");
//		printf("\r\n");
		led_on(&led1);
		Show_Str(0,20,RED,WHITE,"Light:00.00K",16,0);
		LCD_ShowNum(0+6*8,20,(*x)/1000,2,16);
		LCD_ShowNum(0+9*8,20,(*x)%1000,2,16);
		//delay_ms(1000);
	}
}

//���ܴ�����,���ʪ�ȹ��ߣ�����
void pwm_task(void *pdata)
{	u8 err;
	while(1)
	{
		OSSemPend(sig1,0,&err);
		biao+=1;
		TIM_SetCompare3(TIM3,13);
		delay_ms(1000);
	}
}
//����ǿ�Ȳɼ�����
void adc_task(void *pdata)
{
	
	u16 adcx;
	u16 RL;
	while(1){
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		RL=(adcx*10000)/(4096-adcx);
//		printf("�ɼ����Ĺ�������Ϊ��\n");
//		printf("%d\n",RL);
//		printf("��");
//		printf("\r\n");
		Show_Str(0,40,BLUE,WHITE,"Light:00.00K",16,0);
		LCD_ShowNum(0+6*8,40,RL/1000,2,16);
		LCD_ShowNum(0+9*8,40,RL%1000,2,16);
		if(RL>=10000) OSMboxPost(email,(void *)&RL);
		else{
			led_off(&led1);
			LCD_Fill(0,0,128,35,WHITE);
		} 
		delay_ms(1000);
	}
}

//dht11����
void dht11_task(void *pdata)
{
		u8 wendu;
		u8 shidu;
		while(1)
		{
			DHT_Read_Data(&wendu,&shidu,GPIOC,GPIO_Pin_14,&p14);
			Show_Str(0,60,BLUE,WHITE,"Temp:00C",16,0);
			LCD_ShowNum(0+5*8,60,wendu,2,16);
			Show_Str(0,80,BLUE,WHITE,"Humi:00RH",16,0);
			LCD_ShowNum(0+5*8,80,shidu,2,16);
			printf("%d,%d",shidu,wendu);
			if(wendu>=30) OSSemPost(sig2);
			else if(wendu<=24)  TIM_SetCompare3(TIM3,3);
			if(shidu>=70) 
			{
				OSSemPost(sig1);
				if(biao==1)
				{
					biao=0;
					OSTaskSuspend(PWM_TASK_PRIO);
				}
			}
			else if(shidu<=50) OSTaskResume(PWM_TASK_PRIO);
			delay_ms(1000);
		}}

//��������������
void beep_task(void *pdata)
{
		u8 err;
		while(1)
		{
			OSSemPend(sig2,0,&err);
			Beep_on(&bep);
			//delay_ms(5000);
		}
}



void EXTI1_IRQHandler(void)//�ⲿ�ж�1
{
		OSIntEnter();
		delay_ms(50);
		if(KEY0==0)
			{
				flag+=1;
				if(flag%2==1)
				{
					Beep_off(&bep);
					OSTaskSuspend(beep_task_prio);
				}
				else if(flag%2==0)
				{
					 OSTaskResume(beep_task_prio);
					 flag=0;
				}
				Show_Str(0,100,BLUE,WHITE,"Flag:",16,0);
				LCD_ShowNum(0+5*8,100,flag,2,16);
			}
		EXTI_ClearITPendingBit(EXTI_Line1);
		OSIntExit();
}


void EXTI15_10_IRQHandler(void)//�ⲿ�ж�PB12
{
		OSIntEnter();
		delay_ms(50);
	  if(KEY1==0)
	    {
				mark+=1;
				if(mark%2==1)
				{
					led_off(&led1);
					OSTaskSuspend(LED0_TASK_PRIO);
				}
				else if(mark%2==0)
				{
					 OSTaskResume(LED0_TASK_PRIO);
					 mark=0;
				}
				Show_Str(0,120,BLUE,WHITE,"Mark:",16,0);
				LCD_ShowNum(0+5*8,120,mark,2,16);
	    }
		EXTI_ClearITPendingBit(EXTI_Line12);
		OSIntExit();
}



