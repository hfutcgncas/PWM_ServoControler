#include "usart.h"	  
#include "control.h"

#define PKT_BUF_LEN 200
static int PktBuf_index = 0;
static char PktBuf[PKT_BUF_LEN];

extern STmotorCMD motorCMD[NumOfMotor];


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
/**************************实现函数**********************************************
*功    能:		usart1发送一个字节
*********************************************************************************/
void usart1_send(u8 data)
{
	USART1->DR = data;
	while((USART1->SR&0x40)==0);	
}
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置
	
GPIOA->ODR|=1<<2;	

	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.


//=============================

	//使能接收中断
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(0,1,USART1_IRQn,1);//首发命令，需要较高的优先级。 
	

}

/**************************************************************************
函数功能：串口1接收中断
入口参数：无
返回  值：无
**************************************************************************/

void USART1_IRQHandler(void)
{
	u8 ch;
	static u8 ch_last;
	static int inPacket = 0;
	if(USART1->SR&(1<<5))//接收到数据
	{	  
	 static	unsigned char uart_receive=0;//串口接收相关变量
	 uart_receive=USART1->DR; 
	 ch = (u8)uart_receive;
	 
	 if(ch == '\n' || ch == '\r' || ch == '\0')printf(" \r\n"); //换行
	 else USART1->DR = (u8) ch;   //返回显示串口发来的内容
	 
	 if(inPacket)
	 {
		 insertDataPacket(ch);
	 }
	 if(ch_last == 'S' && ch == 'T')
	 {
		 inPacket = 1;
	 }
	 if(ch_last == 'E' && ch == 'N' && inPacket == 1 )
	 {
		 inPacket =0;
		 insertDataPacket('\0');
		 openPacket();
		 PktBuf_index = 0;
	 }		 
	 
	 ch_last = ch;
	 
	}  	
}


void insertDataPacket(char ch)
{
	PktBuf[PktBuf_index] = ch;
	PktBuf_index ++ ;
	if(PktBuf_index >= PKT_BUF_LEN)PktBuf_index = 0;
}


void openPacket()
{
	int motorNO;
	float motorPOS;
	
	float A,Fq,Phi,Offset;
	float A1,A0,Offset1,Offset0;
	
	switch(*PktBuf)
	{
		case('P'):  //位置控制模式
			if(2==sscanf(PktBuf+1,"_#%d_%f_EN",&motorNO,&motorPOS))
			{
					if(motorNO<0 || motorNO >= NumOfMotor )break; //命令有误时跳出
					
					ConfigMotorCMD(&motorCMD[motorNO] , 
													POS,
													motorPOS, 
													motorCMD[motorNO].A,
													motorCMD[motorNO].Offset,
													motorCMD[motorNO].Fq,
													motorCMD[motorNO].phi);		
					
					printf("\r\n cmd :");
					printf("\r\n P #%d %3f \n",motorNO,motorPOS);				
			}
			else
			{
				printf("\r\n Wrong cmd\n");
			}
			break;
		case('W'):
			if(5==sscanf(PktBuf+1,"_#%d_A_%f_O_%f_F_%f_P_%f_EN",&motorNO,&A,&Offset,&Fq,&Phi))
			{
					if(motorNO<0 || motorNO >= NumOfMotor )break; //命令有误时跳出
					
					ConfigMotorCMD(&motorCMD[motorNO] , WAVE, motorCMD[motorNO].Pos, A,Offset,Fq,Phi);
					
					printf("\r\n cmd :");
					printf("\r\n W #%d A %f O %f F %f P %f \n",motorNO,A,Offset,Fq,Phi);
			}
			else
			{
				printf("\r\n Wrong cmd\r\n");
			}
			break;		
		case('S'):
			if( 1 == sscanf(PktBuf+1,"_#%df_EN",&motorNO) )
			{
					if(motorNO<0 || motorNO >= NumOfMotor )break; //命令有误时跳出
					
					ConfigMotorCMD(&motorCMD[motorNO] , 
													STOP,
													motorCMD[motorNO].Pos, 
													motorCMD[motorNO].A,
													motorCMD[motorNO].Offset,
													motorCMD[motorNO].Fq,
													motorCMD[motorNO].phi);		
					printf("\r\n cmd :");
					printf("\r\n Stop #%d \r\n",motorNO);
			}
			else
			{
				printf("\r\n Wrong cmd\r\n");
			}
			break;	
		case('C'):
			if( 6 == sscanf(PktBuf+1,"_A0_%f_O0_%f_A1_%f_O1_%f_F_%f_P_%f_EN",&A0,&Offset0,&A1,&Offset1,&Fq,&Phi) )
			{
					if(NumOfMotor >2 || NumOfMotor <0)
					{
						printf("Wrong motoNO\r\n");
						break;
					}						//电机数量有误时跳出
					
					
					ConfigMotorCMD(&motorCMD[0] , WAVE, motorCMD[0].Pos, A0,Offset0,Fq,motorCMD[0].phi);		
					ConfigMotorCMD(&motorCMD[1] , WAVE, motorCMD[1].Pos, A1,Offset1,Fq,motorCMD[0].phi + Phi);
											
					printf("\r\n cmd :");
					printf("\r\n C \n");
			}
			else
			{
				printf("\r\n Wrong cmd\n");
			}
			break;
		default:
			printf("\r\n Wrong Mode\n");
	}
	
}
