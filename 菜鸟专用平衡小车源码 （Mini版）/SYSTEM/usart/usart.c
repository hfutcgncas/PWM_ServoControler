#include "usart.h"	  

#define PKT_BUF_LEN 200
static int PktBuf_index = 0;
static char PktBuf[PKT_BUF_LEN];


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
/**************************ʵ�ֺ���**********************************************
*��    ��:		usart1����һ���ֽ�
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
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬����
	
GPIOA->ODR|=1<<2;	

	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.


//=============================

	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ� 
	

}

/**************************************************************************
�������ܣ�����1�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/

void USART1_IRQHandler(void)
{
	u8 ch;
	static u8 ch_last;
	static int inPacket = 0;
	if(USART1->SR&(1<<5))//���յ�����
	{	  
	 static	unsigned char uart_receive=0;//���ڽ�����ر���
	 uart_receive=USART1->DR; 
	 ch = (u8)uart_receive;
	 
	 if(ch == '\n' || ch == '\r' || ch == '\0')printf(" \r\n"); //����
	 else USART1->DR = (u8) ch;   //������ʾ���ڷ���������
	 
	 if(inPacket)
	 {
		 insertDataPacket(ch);
	 }
	 if(ch_last == '-' && ch == 'S')
	 {
		 inPacket = 1;
	 }
	 if(ch_last == '-' && ch == 'E' && inPacket == 1 )
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
	float A;
	float F;
	float P;
	float O;
	
	printf("\r\n cmd :");
	sscanf(PktBuf,"A%f_F%f_P%f_O%f-E",&A,&F,&P,&O);	
	printf("\r\n A%3f F%3f P%3f O%3f \n",A,F,P,O);
	
	
}
