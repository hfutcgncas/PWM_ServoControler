#include "control.h"	
#include "filter.h"	
#include "math.h"	
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
extern int PWM_AMP;


int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;

int TIM1_UP_IRQHandler(void)  
{
	static int SysTime = 0;
	if(TIM1->SR&0X0001)//5ms��ʱ�ж�
	{   
		  TIM1->SR&=~(1<<0);                                       //===�����ʱ��1�жϱ�־λ		 
		  Flag_Target=!Flag_Target;
		  if(Flag_Target==1)                                       //5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��
			{
			Get_Angle(Way_Angle);                                    //===������̬	
			return 0;	
			}                                                        //10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
		//	Encoder_Left=Read_Encoder(2);                           //===��ȡ��������ֵ��
		//	Encoder_Right=Read_Encoder(3);                           //===��ȡ��������ֵ
	  	Get_Angle(Way_Angle);                                    //===������̬	
  		Led_Flash(100);                                          //===LED��˸;ָʾ��Ƭ����������	
			Key();                                                   //===ɨ�谴��״̬ ����˫�����Ըı�С������״̬
 	//		Balance_Pwm =balance(Angle_Balance,Gyro_Balance);        //===ƽ��PID����	
	//	  Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);       //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��
 	//    Turn_Pwm    =turn(Encoder_Left,Encoder_Right,Gyro_Turn); //===ת��PID����     
 	//	  Moto1=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                 //===�������ֵ������PWM
 	//  	Moto2=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                 //===�������ֵ������PWM
   		SysTime += 10;
			Moto1 = PWMOut(waveOut( 0.5, 1, 0,  0, SysTime) );
			Moto2 = PWMOut(waveOut( 0.5, 1, (30/180*PI),  0, SysTime));
			Xianfu_Pwm();                                            //===PWM�޷�
  //    if(Turn_Off(Angle_Balance,Voltage)==0)   //===����������쳣
//printf("PWMOUT A : %d",Moto1 );	
//printf("PWMOUT B : %d",Moto2 );	
 			Set_Pwm(Moto1,Moto2);                                    //===��ֵ��PWM�Ĵ���  
	}       	
	 return 0;	  
} 



/**************************************************************************
�������ܣ��������Ǻ����ź�
��ڲ�������ֵ��Ƶ�ʣ���λ�ǣ�ƫת�ǣ�ʱ��
����  ֵ�����PWM
��    �ߣ�����Ƚ
**************************************************************************/
double waveOut( double A, double fq, double phi,  double offset, int t)
{
	const float unit = PWM_AMP/3000.0;  // �������ȳ���PWM���ڣ� �� cnt/us 
	int PWM_w;
	double fac = A* sin(2*PI/fq*(t/1000.0) + phi) + offset ;
	if(fac >1)fac =1;
	else if(fac<-1)fac = -1;

	
	return fac;
}

/**************************************************************************
�������ܣ�����PWM�ź�
��ڲ�����ƫת���� -1~1
����  ֵ�����PWM
��    �ߣ�����Ƚ
**************************************************************************/
int PWMOut( double fac)
{
	const float unit = PWM_AMP/3000.0;  // �������ȳ���PWM���ڣ� �� cnt/us 	
	return (int)((1500 + fac*(1500-900))*unit);
}

/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias,kp=500,kd=0.5;
	 int balance;
	 Bias=Angle-2;              //===���ƽ��ĽǶ���ֵ �ͻ�е��� -0��ζ������������0�ȸ��� �������������5�ȸ��� �Ǿ�Ӧ�ü�ȥ5
	 balance=kp*Bias+Gyro*kd;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ����޸�Movement��ֵ�����磬�ĳ�-60��60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral;
	  float kp=80,ki=0.4;
	  //=============ң��ǰ�����˲���=======================//
		if(1==Flag_Qian)	Movement=90/Flag_sudu;	             //===���ǰ����־λ��1 λ��Ϊ��
		else if(1==Flag_Hou)	  Movement=-90/Flag_sudu;        //===������˱�־λ��1 λ��Ϊ��
	  else  Movement=0;	
   //=============�ٶ�PI������=======================//	
		Encoder_Least =(Encoder_Left+Encoder_Right)-0;  //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
		Encoder *= 0.7;		                         //===һ�׵�ͨ�˲���       
		Encoder += Encoder_Least*0.3;	             //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder;                                  //===���ֳ�λ�� ����ʱ�䣺10ms
		Encoder_Integral=Encoder_Integral-Movement;                  //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;         //===�����޷�
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;         //===�����޷�	
		Velocity=Encoder*kp+Encoder_Integral*ki; //===�ٶȿ���	
		if(Turn_Off(Angle_Balance,Voltage)==1)   Encoder_Integral=0;    //===����رպ��������
	  return Velocity;
}

/**************************************************************************
�������ܣ�ת��PD����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
	  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=88/Flag_sudu,Kp=42,Kd=0;     
	  //=============ң��������ת����=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
		{
			if(++Turn_Count==1)
			Encoder_temp=myabs(encoder_left+encoder_right);
			Turn_Convert=50/Encoder_temp;
			if(Turn_Convert<0.6)Turn_Convert=0.6;
			if(Turn_Convert>3)Turn_Convert=3;
		}	
	  else
		{
			Turn_Convert=0.9;
			Turn_Count=0;
			Encoder_temp=0;
		}			
		if(1==Flag_Left)	           Turn_Target-=Turn_Convert;
		else if(1==Flag_Right)	     Turn_Target+=Turn_Convert; 
		else Turn_Target=0;
	
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  Kd=1;        
		else Kd=0;   //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��
  	//=============ת��PD������=======================//
		  Turn=-Turn_Target*Kp -gyro*Kd;                 //===���Z�������ǽ���PD����
	  return Turn;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
			if(moto1<0)			AIN2=1,			AIN1=0;
			else 	          AIN2=0,			AIN1=1;
			PWMA=myabs(moto1);
		  if(moto2>0)	BIN1=1,			BIN2=0;
			else        BIN1=0,			BIN2=1;
			PWMB=myabs(moto2);	
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=7100;    //===PWM������7200 ������7100
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}
/**************************************************************************
�������ܣ������޸�С������״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)
{	
	u8 tmp;
	tmp=click_N_Double(50); 
	if(tmp==1)Flag_Stop=~Flag_Stop;
	if(tmp==2)Flag_Show=~Flag_Show;
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
	    u8 temp;
			if(angle<-40||angle>40||1==Flag_Stop)
			{	                                                 //===��Ǵ���40�ȹرյ��
      temp=1;                                            //===Flag_Stop��1�رյ��
			AIN1=0;                                            //===���������������¶ȹ���ʱ�رյ��
			AIN2=0;
			BIN1=0;
			BIN2=0;
      }
			else
      temp=0;
      return temp;			
}
	
/**************************************************************************
�������ܣ���ȡ�Ƕ�
��ڲ�������ȡ�Ƕȵ��㷨 1����  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(u8 way)
{ 
	    float Accel_Y,Accel_X,Accel_Z,Gyro_Y,Gyro_Z;
	    if(way==1)                                      //DMPû���漰���ϸ��ʱ�����⣬����������ȡ
			{	
			}			
      else
      {
			Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //��ȡY��������
			Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ��������
		  Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ�
	  	Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�
		  if(Gyro_Y>32768)  Gyro_Y-=65536;     //��������ת��  Ҳ��ͨ��shortǿ������ת��
			if(Gyro_Z>32768)  Gyro_Z-=65536;     //��������ת��
	  	if(Accel_X>32768) Accel_X-=65536;    //��������ת��
		  if(Accel_Z>32768) Accel_Z-=65536;    //��������ת��
			Gyro_Balance=-Gyro_Y;                                  //����ƽ����ٶ�
	   	Accel_Y=atan2(Accel_X,Accel_Z)*180/PI;                 //���������ļн�	
		  Gyro_Y=Gyro_Y/16.4;                                    //����������ת��	
      if(Way_Angle==2)		  	Kalman_Filter(Accel_Y,-Gyro_Y);//�������˲�	
			else if(Way_Angle==3)   Yijielvbo(Accel_Y,-Gyro_Y);    //�����˲�
	    Angle_Balance=angle;                                   //����ƽ�����
			Gyro_Turn=Gyro_Z;                                      //����ת����ٶ�
	  	}
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
