#include "motor.h"
void MiniBalance_Motor_Init(void)
{
	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ��   
	GPIOB->CRL&=0X0000FFFF;   //PORTB4 5 6 7�������
	GPIOB->CRL|=0X22220000;   //PORTB4 5 6 7�������
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 					 
	MiniBalance_Motor_Init(); //��ʼ�������������IO
	RCC->APB1ENR|=1<<2;       //TIM4ʱ��ʹ��    
	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ��     
	GPIOB->CRH&=0XFFFFFF00;   //PORTB8 9�������
	GPIOB->CRH|=0X000000BB;   //PORTB8 9�������
	TIM4->ARR=arr;//�趨�������Զ���װֵ 
	TIM4->PSC=psc;//Ԥ��Ƶ������Ƶ
	TIM4->CCMR2|=6<<12;//CH4 PWM1ģʽ	
	TIM4->CCMR2|=6<<4; //CH3 PWM1ģʽ	
	TIM4->CCMR2|=1<<11;//CH4Ԥװ��ʹ��	 
	TIM4->CCMR2|=1<<3; //CH3Ԥװ��ʹ��	  
	TIM4->CCER|=1<<12; //CH4���ʹ��	   
	TIM4->CCER|=1<<8;  //CH3���ʹ��	
	TIM4->CR1=0x8000;  //ARPEʹ�� 
	TIM4->CR1|=0x01;   //ʹ�ܶ�ʱ��4				

//���ｫPWM�����д��Ϊ��ֵ����֤��bootloader�ж�����ᳬ�ޡ�
TIM4->CCR3 = 3600;
TIM4->CCR4 = 3600;
} 

