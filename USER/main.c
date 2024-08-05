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


/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//LED任务
//设置任务优先级
#define LED0_TASK_PRIO       			6
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		512
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);


//智能窗任务
//设置任务优先级
#define PWM_TASK_PRIO       			7
//设置任务堆栈大小
#define PWM_STK_SIZE  					64
//任务堆栈
OS_STK PWM_TASK_STK[PWM_STK_SIZE];
//任务函数
void pwm_task(void *pdata);

//光敏电阻任务
#define ADC_TASK_PRIO            9
#define ADC_STK_SIZE						512
OS_STK  ADC_TASK_STK[ADC_STK_SIZE];
void 		adc_task(void *pdata);

//dht11任务
#define dht11_task_prio 				8
#define dht11_stk_size  			  512
OS_STK dht11_task_stk[dht11_stk_size];
void dht11_task(void *pdata);

//蜂鸣器开启任务
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    //延时函数初始化	
	//SCB->VTOR = FLASH_BASE | 0x4000;	 
	LED_Init(&led1,GPIOC,GPIO_Pin_13);			 //初始化与LED连接的硬件接口
	Beep_Init(&bep,GPIOB,GPIO_Pin_15);
	TIM3_PWM_Init(99,14399);
	Adc_Init();
	EXTIX_Init();
	LCD_Init();	
	uart_init(9600);
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	  	 
}
	  
//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	//Bsem=OSSemCreate(0);
	sig1=OSSemCreate(0);
	sig2=OSSemCreate(0);
	email=OSMboxCreate((void *)0);
  OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);	//为什么这里的第二个参数是空指针呢？因为这个参数是要赋值给R0寄存器的					   
 	OSTaskCreate(pwm_task,(void *)0,(OS_STK*)&PWM_TASK_STK[PWM_STK_SIZE-1],PWM_TASK_PRIO);	//而R0寄存器是堆栈指针PSP，如果PSP=0，代表任务第一次进行任务切换，所以
	OSTaskCreate(adc_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_TASK_PRIO);//创建任务时这个参数要为0
	OSTaskCreate(dht11_task,(void *)0,(OS_STK*)&dht11_task_stk[dht11_stk_size-1],dht11_task_prio);
	OSTaskCreate(beep_task,(void *)0,(OS_STK*)&beep_task_stk[beep_stk_size-1],beep_task_prio);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//LED任务
void led0_task(void *pdata)
{	
	u8 err; 	
	u16 *x;
	while(1)
	{
		x=(u16 *)OSMboxPend(email,0,&err);
//		Show_Str(20,30,BLUE,WHITE,"Light:      C",16,0);
//		LCD_ShowNum(20+6*8,30,*x,2,16);
//		printf("此时的光敏电阻值为：");
//		printf("%d\n",*x);
//		printf("Ω");
//		printf("\r\n");
		led_on(&led1);
		Show_Str(0,20,RED,WHITE,"Light:00.00K",16,0);
		LCD_ShowNum(0+6*8,20,(*x)/1000,2,16);
		LCD_ShowNum(0+9*8,20,(*x)%1000,2,16);
		//delay_ms(1000);
	}
}

//智能窗任务,如果湿度过高，开窗
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
//光照强度采集任务
void adc_task(void *pdata)
{
	
	u16 adcx;
	u16 RL;
	while(1){
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		RL=(adcx*10000)/(4096-adcx);
//		printf("采集到的光敏电阻为：\n");
//		printf("%d\n",RL);
//		printf("Ω");
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

//dht11任务
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

//蜂鸣器开启任务
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



void EXTI1_IRQHandler(void)//外部中断1
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


void EXTI15_10_IRQHandler(void)//外部中断PB12
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



