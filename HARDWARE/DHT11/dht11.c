#include "dht11.h"
#include "led.h"

u8  BUF[5]={0x00,0x00,0x00,0x00,0x00};    //�洢��ȡ����ʪ����Ϣ
u32 sum=0;         //У��ֵ

 void DHT11_Start(led_d *io)  //DHT11������������PC14
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
	chushi(io);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������ģʽ
	GPIO_InitStructure.GPIO_Pin = io->pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(io->port, &GPIO_InitStructure);
	GPIO_ResetBits(io->port,io->pin); //����������
  delay_ms(20);    										//��������18ms
  GPIO_SetBits(io->port,io->pin); 	//���������� 
	delay_us(30);     									//��������20~40us
	GPIO_ResetBits(io->port,io->pin);

}

void DHT11_Read(led_d *io)

{
	GPIO_InitTypeDef GPIO_InitStructure;
	chushi(io);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //�������ģʽ
	GPIO_InitStructure.GPIO_Pin = io->pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(io->port, &GPIO_InitStructure);
}


u8 DHT_Read_Byte(led_d *io)
{
	u8 temp;  
	u8 ReadDat=0; 
	u8 t = 0;
	u8 i; 
 
	for(i=0;i<8;i++)
	{
		while(readpin(io)==0&&t<100)  
		{		
			delay_us(1);
			t++;  //��ֹ����
		}
		t=0;
		//���ڡ�0������ߵ�ƽʱ��26~28us��1����ߵ�ƽʱ��70us����ʱ30us�����жϸߵ͵�ƽ������0��ȡ�����ǵ͵�ƽ���ߵ�ƽ��֮
		delay_us(30);
		temp=0;
		if(readpin(io)==1) temp=1;		
			
		while(readpin(io)==1&&t<100)
		{		
			delay_us(1);
			t++;
		}
		t=0;
		ReadDat<<=1; //����1λ��DHT11�����ɸߵ��ͷ���
		ReadDat|=temp;
	}	
	return ReadDat;
}
 
u8 DHT_Read_Data(u8 *temp,u8 *humi,gpioled port,u16 pin,led_d *io)
{
	u8 i;
	u8 t = 0;
	io->port=port;
	io->pin=pin;
	DHT11_Start(io);
	DHT11_Read(io);
	delay_us(20);
	//��ʱ20us���͵�ƽ80us����ʣ60us������Ƿ��ǵ͵�ƽ��ȷ���Ƿ�����Ӧ�ź�
	if(readpin(io)==0)  //�����ȡ���͵�ƽ��֤��DHT11��Ӧ
	{
		while(readpin(io)==0&&t<100)//������Ӧ�źŵ͵�ƽʣ��60us���ȴ���ߵ�ƽ
		{
			delay_us(1);
			t++;			
		}
		t=0;//����100us�Զ��������У����⿨��
		while(readpin(io)==1&&t<100)//������Ӧ�źŸߵ�ƽ80us���ȴ���͵�ƽ
			{
				delay_us(1);
				t++;			
			}
		t=0;
		for(i=0;i<5;i++)  //����40λ����
			{
				BUF[i]=DHT_Read_Byte(io);//����1���ֽ�
			}
		delay_us(50);//�����ź�
	}
	sum=BUF[0]+BUF[1]+BUF[2]+BUF[3];		// BUF[0]��ʪ������ֵ BUF[1]��ʪ��С��ֵ BUF[2]���¶�����ֵ BUF[3]���¶�С��ֵ BUF[4]��У��λ����Ϊ8λ����
	if(BUF[4]==(u8)sum)  //У��  
	{
		*humi=BUF[0];
		*temp=BUF[2];
		return 1;    
	}
	else
		return 0;    
}

void chushi(led_d *io)
{
		if(io->port==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
		else if(io->port==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		else if(io->port==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		else if(io->port==GPIOD) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		else if(io->port==GPIOE) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		else if(io->port==GPIOF) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
		else if(io->port==GPIOG) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

}

u8 readpin(led_d *io)
{
	return GPIO_ReadInputDataBit(io->port,io->pin);
}
