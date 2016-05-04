#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "stdio.h"	 
void usart1_send(u8 data);
void uart_init(u32 pclk2,u32 bound);
void USART1_IRQHandler(void);

void insertDataPacket(char ch);
int openPacket(void);
#endif	   
















