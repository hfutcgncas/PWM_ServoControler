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

	if(USART1->SR&(1<<5))//���յ�����
	{	  
	 static	unsigned char uart_receive=0;//���ڽ�����ر���
	 uart_receive=USART1->DR; 
	 ch = (u8)uart_receive;
	 
	 if(ch == '\n' || ch == '\r' || ch == '\0') //һ���������
	 {
			insertDataPacket('\0');
			
			openPacket();
			PktBuf_index = 0;
			printf(" \r\n >>"); //����,���¿�ʼ����
	 }
	 else
	 {
		 USART1->DR = (u8) ch;   //������ʾ���ڷ���������
		 insertDataPacket(ch);
	 }		 
	}  	
}


void insertDataPacket(char ch)
{
	PktBuf[PktBuf_index] = ch;
	PktBuf_index ++ ;
	if(PktBuf_index >= PKT_BUF_LEN)PktBuf_index = 0;
}


int openPacket()
{
	int motorNO;
	float motorPOS;
	float motorPOS0,motorPOS1,motorPOS2,motorPOS3;
	
	float A,Fq,Phi,Offset;
	float A0,Offset0;
	float A1,Offset1,Phi1;
	float A2,Offset2,Phi2;
	float A3,Offset3,Phi3;
	
	char cmdPara;
	float cmdVal;
	
	int index;
	
	if(PktBuf[0] == '\0')return 0; //������
	
	printf(" \r\n"); //����
	if(strcmp(PktBuf,"HELP") == 0 || strcmp(PktBuf,"help") == 0)
	{
		printf("\r\n =================== Command List =================== \r\n");
		printf("\r\n");
		printf("\r\n  0- RESTART:                RESTART \r\n");
		printf("\r\n  1- STOP:                   STOP #n (n = 0~3 or \"ALL\") \r\n");
		printf("\r\n  2- POSE SINGLE:            POSE #n p (n = 0~3, p=-1~1) \r\n");
		printf("\r\n  3- POSE ALL(pre Setted):   POSE #ALL  \r\n");
		printf("\r\n  4- POSE ALL(SET NOW):      POSE ALL p0 p1 p2 p3 (pi = -1 ~ 1)  \r\n");
		printf("\r\n  5- WAVE SINGLE:            WAVE #n (n = 0~3) \r\n");
		printf("\r\n  6- WAVE ALL(pre Setted):   WAVE #ALL  \r\n");
		printf("\r\n  7- WAVE ALL(SET NOW):      WAVE ALL -A0 a0 -O0 o0 -A1 a1 -O1 o1 -D1 phi1 -A2 a2 -O2 o2 -D2 phi2 -A3 a3 -O3 o3 -D3 phi3 -F fequance");
		printf("\r\n  8- List Motor paras:       LIST #n  (n = 0~3) \r\n");
		printf("\r\n  9- Config Motor:           CFG #n -c val (n = 0~3)");
		printf("\r\n                                            c = 'P': POSE        val=(-1~1)");
		printf("\r\n                                                'A': Amplitude   val=( 0~1)");
		printf("\r\n                                                'O': Offset      val=(-1~1)");
		printf("\r\n                                                'D': phase angle val=(-PI~PI)");
		printf("\r\n                                                'F': fequance    val=(0.25~1.5)");
				
		//printf("\r\n  2- WAVE MODE: WAVE #n -A a -O offset -F fequance -P phi \r\n");
		//printf("\r\n  3- LINKAGE MODE: LINKAGE -A0 a0 -O0 o0 -A1 a1 -O1 o1 -P1 phi1 -A2 a2 -O2 o2 -P2 phi2 -A3 a3 -O3 o3 -P3 phi3 -F fequance \r\n");
		printf("\r\n");
		printf("\r\n ==================================================== \r\n");
	}
	else if(strcmp(PktBuf,"RESTART") == 0)
	{
		Sys_Soft_Reset();
	}
	else if(strcmp(PktBuf,"POSE #ALL") == 0)
	{
			for(index = 0;index<NumOfMotor;index++)
			{
							ConfigMotorCMD(&motorCMD[index] , 
													POS,
													motorCMD[index].Pos, 
													motorCMD[index].A,
													motorCMD[index].Offset,
													motorCMD[index].Fq,
													motorCMD[index].phi);	
		  }
											printf("\r\n cmd :");
					printf("\r\n #ALL POSE \n");
	}
	else if(strcmp(PktBuf,"WAVE #ALL") == 0)
	{
			for(index = 0;index<NumOfMotor;index++)
			{
							ConfigMotorCMD(&motorCMD[index] , 
													WAVE,
													motorCMD[index].Pos, 
													motorCMD[index].A,
													motorCMD[index].Offset,
													motorCMD[index].Fq,
													motorCMD[index].phi);	
		  }			
								printf("\r\n cmd :");
					printf("\r\n #ALL WAVE \n");
	}
	else if(strcmp(PktBuf,"STOP #ALL") == 0)
	{
			for(index = 0;index<NumOfMotor;index++)
			{
							ConfigMotorCMD(&motorCMD[index] , 
													STOP,
													motorCMD[index].Pos, 
													motorCMD[index].A,
													motorCMD[index].Offset,
													motorCMD[index].Fq,
													motorCMD[index].phi);	
		  }		
								printf("\r\n cmd :");
					printf("\r\n #ALL STOP \n");			
	}
	else if(1 == sscanf(PktBuf,"STOP #%d ",&motorNO) )
	{
					if(motorNO<0 || motorNO >= NumOfMotor )return 0; //��������ʱ����
					
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
	else if(2 == sscanf(PktBuf,"POSE #%d %f",&motorNO,&motorPOS) )
	{
		
					if(motorNO<0 || motorNO >= NumOfMotor )return 0; //��������ʱ����
					
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
	else if(1==sscanf(PktBuf,"WAVE #%d",&motorNO))
	{		
					if(motorNO<0 || motorNO >= NumOfMotor )return 0; //��������ʱ����
					
					ConfigMotorCMD(&motorCMD[motorNO] , 
													WAVE,
													motorCMD[motorNO].Pos, 
													motorCMD[motorNO].A,
													motorCMD[motorNO].Offset,
													motorCMD[motorNO].Fq,
													motorCMD[motorNO].phi);	
					
					printf("\r\n cmd :");
					printf("\r\n W #%d A %f O %f F %f D %f \n",motorNO,motorCMD[motorNO].A,motorCMD[motorNO].Offset,motorCMD[motorNO].Fq,motorCMD[motorNO].phi);
	}
	else if( 3 == sscanf(PktBuf,"CFG #%d -%c %f",&motorNO,&cmdPara,&cmdVal) )
	{
					if(NumOfMotor >4 || NumOfMotor <0)
					{
						printf("Wrong motoNO\r\n");
						return 0;
					}						//�����������ʱ����
					
					switch(cmdPara)				
					{
						case 'P':
							ConfigMotorCMD(&motorCMD[motorNO] ,
															STOP, 
															cmdVal,
															motorCMD[motorNO].A,
															motorCMD[motorNO].Offset,
															motorCMD[motorNO].Fq,
															motorCMD[motorNO].phi);	
							break;
						case 'A':
							ConfigMotorCMD(&motorCMD[motorNO] ,
															STOP, 
															motorCMD[0].Pos,
															cmdVal,
															motorCMD[motorNO].Offset,
															motorCMD[motorNO].Fq,
															motorCMD[motorNO].phi);	
							break;
						case 'O':
							ConfigMotorCMD(&motorCMD[motorNO] ,
															STOP, 
															motorCMD[0].Pos,
															motorCMD[motorNO].A,
															cmdVal,
															motorCMD[motorNO].Fq,
															motorCMD[motorNO].phi);	
							break;
						case 'F':
							ConfigMotorCMD(&motorCMD[motorNO] ,
															STOP, 
															motorCMD[0].Pos,
															motorCMD[motorNO].A,
															motorCMD[motorNO].Offset,
															cmdVal,
															motorCMD[motorNO].phi);	
							break;
						case 'D':
							ConfigMotorCMD(&motorCMD[motorNO] ,
															STOP, 
															motorCMD[0].Pos,
															motorCMD[motorNO].A,
															motorCMD[motorNO].Offset,
															motorCMD[motorNO].Fq,
															cmdVal);	
							break;
					}											
					printf("\r\n cmd :");
					printf("\r\n CFG \n");
	}
	else if(1==sscanf(PktBuf,"LIST #%d",&motorNO))
	{		
					if(motorNO<0 || motorNO >= NumOfMotor )return 0; //��������ʱ����
					
					printf("ID          %d \r\n",motorNO); 
					switch(motorCMD[motorNO].Mode)
					{
						case(POS):
							printf("MODE        POSE \r\n");
							break;
						case(WAVE):
							printf("MODE        WAVE \r\n");
							break;
						case(STOP):
							printf("MODE        STOP \r\n");
							break;
						default:
							printf("MODE        error \r\n");
					}
					 
					printf("POSE        %f \r\n",motorCMD[motorNO].Pos); 
					printf("Amplitude   %f \r\n",motorCMD[motorNO].A); 
					printf("Offset      %f \r\n",motorCMD[motorNO].Offset); 
					printf("fequance    %f \r\n",motorCMD[motorNO].Fq); 
					printf("phase angle %f \r\n",motorCMD[motorNO].phi); 
	}
	else if( 12 == sscanf(PktBuf,"WAVE ALL -A0 %f -O0 %f -A1 %f -O1 %f -P1 %f -A2 %f -O2 %f -P2 %f -A3 %f -O3 %f -P3 %f -F %f",
													&A0,&Offset0,
													&A1,&Offset1,&Phi1,
													&A2,&Offset2,&Phi2,
													&A3,&Offset3,&Phi3, 
													&Fq) )
	{
					if(NumOfMotor >4 || NumOfMotor <0)
					{
						printf("Wrong motoNO\r\n");
						return 0;
					}						//�����������ʱ����
						
					ConfigMotorCMD(&motorCMD[0] , WAVE, motorCMD[0].Pos, A0,Offset0,Fq,motorCMD[0].phi);		
					ConfigMotorCMD(&motorCMD[1] , WAVE, motorCMD[1].Pos, A1,Offset1,Fq,motorCMD[0].phi + Phi1);
					ConfigMotorCMD(&motorCMD[2] , WAVE, motorCMD[2].Pos, A2,Offset2,Fq,motorCMD[0].phi + Phi2);
					ConfigMotorCMD(&motorCMD[3] , WAVE, motorCMD[3].Pos, A3,Offset3,Fq,motorCMD[0].phi + Phi3);
											
					printf("\r\n cmd :");
					printf("\r\n WAVE #ALL \n");
	}
	else if( 4 == sscanf(PktBuf,"POSE ALL %f %f %f %f",&motorPOS0,&motorPOS1,&motorPOS2,&motorPOS3) )
	{
					if(NumOfMotor >4 || NumOfMotor <0)
					{
						printf("Wrong motoNO\r\n");
						return 0;
					}						//�����������ʱ����
	
					ConfigMotorCMD(&motorCMD[0] , POS, motorPOS3, motorCMD[0].A,motorCMD[0].Offset,motorCMD[0].Fq,motorCMD[0].phi);
					ConfigMotorCMD(&motorCMD[1] , POS, motorPOS3, motorCMD[1].A,motorCMD[1].Offset,motorCMD[1].Fq,motorCMD[1].phi);
					ConfigMotorCMD(&motorCMD[2] , POS, motorPOS3, motorCMD[2].A,motorCMD[2].Offset,motorCMD[2].Fq,motorCMD[2].phi);
					ConfigMotorCMD(&motorCMD[3] , POS, motorPOS3, motorCMD[3].A,motorCMD[3].Offset,motorCMD[3].Fq,motorCMD[3].phi);
											
					printf("\r\n cmd :");
					printf("\r\n POSE #ALL \n");
	}
	else
	{
			printf("\r\n Wrong cmd\n");
			return 0;
	}	
	return 1;
}
