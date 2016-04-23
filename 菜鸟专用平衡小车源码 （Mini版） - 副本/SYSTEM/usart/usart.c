#include "usart.h"	  
#include "control.h"

#define PKT_BUF_LEN 200
static int PktBuf_index = 0;
static char PktBuf[PKT_BUF_LEN];

extern STmotorCMD motorCMD[NumOfMotor];


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
	MY_NVIC_Init(0,1,USART1_IRQn,1);//�׷������Ҫ�ϸߵ����ȼ��� 
	

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
		case('P'):  //λ�ÿ���ģʽ
			if(2==sscanf(PktBuf+1,"_#%d_%f_EN",&motorNO,&motorPOS))
			{
					if(motorNO<0 || motorNO >= NumOfMotor )break; //��������ʱ����
					
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
					if(motorNO<0 || motorNO >= NumOfMotor )break; //��������ʱ����
					
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
					if(motorNO<0 || motorNO >= NumOfMotor )break; //��������ʱ����
					
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
					}						//�����������ʱ����
					
					
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
