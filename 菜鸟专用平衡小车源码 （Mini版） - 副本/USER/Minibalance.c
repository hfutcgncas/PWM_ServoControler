#include "sys.h"
#include "control.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
#define PWM_FQ 10000

u8 Way_Angle=2;                             //��ȡ�Ƕȵ��㷨��1����Ԫ��  2��������  3�������˲� Ĭ�ϴ��ؿ������˲�
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //����ң����صı���
u8 Flag_Stop=0,Flag_Show=1;                 //ֹͣ��־λ�� ��ʾ��־λ Ĭ��ֹͣ ��ʾ��
int Encoder_Left,Encoder_Right;             //���ұ��������������
int Moto1,Moto2;                            //���PWM���� Ӧ��Motor�� ��Moto�¾�	
int Temperature;                            //��ʾ�¶�
int Voltage;                                //��ص�ѹ������صı���
float Angle_Balance,Gyro_Balance,Gyro_Turn; //ƽ����� ƽ�������� ת��������
float Show_Data_Mb;                         //ȫ����ʾ������������ʾ��Ҫ�鿴������

const int PWM_AMP = 7200 ;

int main(void)
{ 
	//NVIC_SetVectorTable();
	Stm32_Clock_Init(9);            //ϵͳʱ������
	delay_init(72);                 //��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
	LED_Init();                     //��ʼ���� LED ���ӵ�Ӳ���ӿ�
	KEY_Init();                     //������ʼ��
	//OLED_Init();                    //OLED��ʼ��
	uart_init(72,115200);           //��ʼ������1
  uart2_init(36,9600);            //����2��ʼ��
	MiniBalance_PWM_Init(PWM_AMP-1,30-1);   //=====��ʼ��PWM �������������  Ƶ��72M/(7199+1)/30 = 333Hz 
//	Encoder_Init_TIM2();            //=====�������ӿ�
//	Encoder_Init_TIM3();            //��ʼ��������2 
	IIC_Init();                     //ģ��IIC��ʼ��
  MPU6050_initialize();           //=====MPU6050��ʼ��	
	DMP_Init();                     //��ʼ��DMP    
  Timer1_Init(49,7199);           //=====5MS��һ���жϷ�����
	
	MyMotorInt();  // ��ʼ������Ŀ��Ʋ���
	while(1)
		{
       
					Temperature=Read_Temperature();  //===��ȡMPU6050�����¶ȴ��������ݣ����Ʊ�ʾ�����¶ȡ�	
				//	oled_show(); //===��ʾ����
				//  printf("ƽ�����%f  ���ֱ�����%d  ���ֱ�����%d  ��ص�ѹ%dmV\r\n",Angle_Balance,Encoder_Left,Encoder_Right,Voltage);//����λ����������
					delay_ms(100);	//��ʱ�������ݴ���Ƶ�ʣ�ȷ��ͨ�ŵ��ȶ�

		} 
}
