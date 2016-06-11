
#include"stm32f10x_lib.h"
#include"RCC_Configuration.h"  //ϵͳʱ������
#include"USART_Configuration.h"
#include"delay.h"
#include <stdio.h>
TIM_OCInitTypeDef TIM_OCInitStructure;
u16	 aa, flay=0	;

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	//���������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //������ʱ��ʱ��
	TIM_DeInit(TIM3);									   //��ʼֵ
	TIM_TimeBaseStructure.TIM_Prescaler=0;             //TIM3ʱ�ӵ���Ƶ��  
	TIM_TimeBaseStructure.TIM_Period=600;                 // װ���ֵ ����Ƶ�� 50ms
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //ʱ�ӷָ� ���Բ�Ҫ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;		//PWMģʽ2
	TIM_OCInitStructure.TIM_Pulse=aa; 					//����ռ����
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;	 //ʹ������Ƚ�
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		//�������
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);			//װ��TIM3_CH1
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);			//װ��TIM3_CH2
//	TIM_ARRPreloadConfig(TIM3,ENABLE);					//ʹ��TIM3��ARR�ϵ�Ԥװ�ڼĴ��������Բ�Ҫ
	TIM_Cmd(TIM3,ENABLE);								//ʹ��TIM3����
}
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	USART_Configuration();
	TIM3_Configuration();
	delay_init(72);
	while(1)
	{	
		if(flay==0)
		{
			aa++;						   // �Լ��ı�ռ�ձ�
			if(aa==600)
			{
				flay=1;
			}	
		}
		if(flay==1)
			{
				aa--;
				if(aa==0)
				{
					flay=0;
				}	
			}				 
	   delay_ms(1);				   //��ʱ
	   	printf("\r\n aa=  %d\r\n",aa);
	   TIM_OCInitStructure.TIM_Pulse=aa;
	   TIM_OC3Init(TIM3, &TIM_OCInitStructure);	  //����װ��ռ�ձ�ֵ
	   TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	}
		
}




