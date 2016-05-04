#include "led.h"
//初始化 PC13 为输出口.并使能PORTA口的时钟

void LED_Init(void)
{
//RCC->APB2ENR|=1<<2; //使能 PORTA 时钟  
RCC->APB2ENR|=1<<4;     	//使能PORTC时钟
GPIOC->CRH&=0XFFFFFFF0;
GPIOC->CRH|=0X00000003;//PA8 推挽输出
GPIOC->ODR|=1<<13; //PC 输出高
}

/**************************************************************************
函数功能：LED闪烁
入口参数：闪烁频率 
返回  值：无
**************************************************************************/
void Led_Flash(u16 time)
{
	  static int temp;
		if(++temp==time)	LED=~LED,temp=0;
}
