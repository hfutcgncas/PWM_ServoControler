#include "control.h"	
#include "filter.h"	
#include "math.h"	
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
extern int PWM_AMP;


int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;

int TIM1_UP_IRQHandler(void)  
{
	static int SysTime = 0;
	if(TIM1->SR&0X0001)//5ms定时中断
	{   
		  TIM1->SR&=~(1<<0);                                       //===清除定时器1中断标志位		 
		  Flag_Target=!Flag_Target;
		  if(Flag_Target==1)                                       //5ms读取一次陀螺仪和加速度计的值，更高的采样频率可以改善卡尔曼滤波和互补滤波的效果
			{
			Get_Angle(Way_Angle);                                    //===更新姿态	
			return 0;	
			}                                                        //10ms控制一次，为了保证M法测速的时间基准，首先读取编码器数据
		//	Encoder_Left=Read_Encoder(2);                           //===读取编码器的值，
		//	Encoder_Right=Read_Encoder(3);                           //===读取编码器的值
	  	Get_Angle(Way_Angle);                                    //===更新姿态	
  		Led_Flash(100);                                          //===LED闪烁;指示单片机正常运行	
			Key();                                                   //===扫描按键状态 单击双击可以改变小车运行状态
 	//		Balance_Pwm =balance(Angle_Balance,Gyro_Balance);        //===平衡PID控制	
	//	  Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);       //===速度环PID控制	 记住，速度反馈是正反馈，就是小车快的时候要慢下来就需要再跑快一点
 	//    Turn_Pwm    =turn(Encoder_Left,Encoder_Right,Gyro_Turn); //===转向环PID控制     
 	//	  Moto1=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                 //===计算左轮电机最终PWM
 	//  	Moto2=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                 //===计算右轮电机最终PWM
   		SysTime += 10;
			Moto1 = PWMOut(waveOut( 0.5, 1, 0,  0, SysTime) );
			Moto2 = PWMOut(waveOut( 0.5, 1, (30/180*PI),  0, SysTime));
			Xianfu_Pwm();                                            //===PWM限幅
  //    if(Turn_Off(Angle_Balance,Voltage)==0)   //===如果不存在异常
//printf("PWMOUT A : %d",Moto1 );	
//printf("PWMOUT B : %d",Moto2 );	
 			Set_Pwm(Moto1,Moto2);                                    //===赋值给PWM寄存器  
	}       	
	 return 0;	  
} 



/**************************************************************************
函数功能：生成三角函数信号
入口参数：幅值，频率，相位角，偏转角，时间
返回  值：舵机PWM
作    者：刘健冉
**************************************************************************/
double waveOut( double A, double fq, double phi,  double offset, int t)
{
	const float unit = PWM_AMP/3000.0;  // 计数长度除以PWM周期， 得 cnt/us 
	int PWM_w;
	double fac = A* sin(2*PI/fq*(t/1000.0) + phi) + offset ;
	if(fac >1)fac =1;
	else if(fac<-1)fac = -1;

	
	return fac;
}

/**************************************************************************
函数功能：生成PWM信号
入口参数：偏转参数 -1~1
返回  值：舵机PWM
作    者：刘健冉
**************************************************************************/
int PWMOut( double fac)
{
	const float unit = PWM_AMP/3000.0;  // 计数长度除以PWM周期， 得 cnt/us 	
	return (int)((1500 + fac*(1500-900))*unit);
}

/**************************************************************************
函数功能：直立PD控制
入口参数：角度、角速度
返回  值：直立控制PWM
作    者：平衡小车之家
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias,kp=500,kd=0.5;
	 int balance;
	 Bias=Angle-2;              //===求出平衡的角度中值 和机械相关 -0意味着身重中心在0度附近 如果身重中心在5度附近 那就应该减去5
	 balance=kp*Bias+Gyro*kd;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	 return balance;
}

/**************************************************************************
函数功能：速度PI控制 修改前进后退速度，请修改Movement的值，比如，改成-60和60就比较慢了
入口参数：左轮编码器、右轮编码器
返回  值：速度控制PWM
作    者：平衡小车之家
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral;
	  float kp=80,ki=0.4;
	  //=============遥控前进后退部分=======================//
		if(1==Flag_Qian)	Movement=90/Flag_sudu;	             //===如果前进标志位置1 位移为负
		else if(1==Flag_Hou)	  Movement=-90/Flag_sudu;        //===如果后退标志位置1 位移为正
	  else  Movement=0;	
   //=============速度PI控制器=======================//	
		Encoder_Least =(Encoder_Left+Encoder_Right)-0;  //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
		Encoder *= 0.7;		                         //===一阶低通滤波器       
		Encoder += Encoder_Least*0.3;	             //===一阶低通滤波器    
		Encoder_Integral +=Encoder;                                  //===积分出位移 积分时间：10ms
		Encoder_Integral=Encoder_Integral-Movement;                  //===接收遥控器数据，控制前进后退
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;         //===积分限幅
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;         //===积分限幅	
		Velocity=Encoder*kp+Encoder_Integral*ki; //===速度控制	
		if(Turn_Off(Angle_Balance,Voltage)==1)   Encoder_Integral=0;    //===电机关闭后清除积分
	  return Velocity;
}

/**************************************************************************
函数功能：转向PD控制
入口参数：左轮编码器、右轮编码器、Z轴陀螺仪
返回  值：转向控制PWM
作    者：平衡小车之家
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//转向控制
{
	  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=88/Flag_sudu,Kp=42,Kd=0;     
	  //=============遥控左右旋转部分=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
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
	
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===转向速度限幅
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  Kd=1;        
		else Kd=0;   //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
  	//=============转向PD控制器=======================//
		  Turn=-Turn_Target*Kp -gyro*Kd;                 //===结合Z轴陀螺仪进行PD控制
	  return Turn;
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
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
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=7100;    //===PWM满幅是7200 限制在7100
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}
/**************************************************************************
函数功能：按键修改小车运行状态 
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{	
	u8 tmp;
	tmp=click_N_Double(50); 
	if(tmp==1)Flag_Stop=~Flag_Stop;
	if(tmp==2)Flag_Show=~Flag_Show;
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：倾角和电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(float angle, int voltage)
{
	    u8 temp;
			if(angle<-40||angle>40||1==Flag_Stop)
			{	                                                 //===倾角大于40度关闭电机
      temp=1;                                            //===Flag_Stop置1关闭电机
			AIN1=0;                                            //===可自行增加主板温度过高时关闭电机
			AIN2=0;
			BIN1=0;
			BIN2=0;
      }
			else
      temp=0;
      return temp;			
}
	
/**************************************************************************
函数功能：获取角度
入口参数：获取角度的算法 1：无  2：卡尔曼 3：互补滤波
返回  值：无
**************************************************************************/
void Get_Angle(u8 way)
{ 
	    float Accel_Y,Accel_X,Accel_Z,Gyro_Y,Gyro_Z;
	    if(way==1)                                      //DMP没有涉及到严格的时序问题，在主函数读取
			{	
			}			
      else
      {
			Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //读取Y轴陀螺仪
			Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //读取Z轴陀螺仪
		  Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //读取X轴加速度记
	  	Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //读取Z轴加速度记
		  if(Gyro_Y>32768)  Gyro_Y-=65536;     //数据类型转换  也可通过short强制类型转换
			if(Gyro_Z>32768)  Gyro_Z-=65536;     //数据类型转换
	  	if(Accel_X>32768) Accel_X-=65536;    //数据类型转换
		  if(Accel_Z>32768) Accel_Z-=65536;    //数据类型转换
			Gyro_Balance=-Gyro_Y;                                  //更新平衡角速度
	   	Accel_Y=atan2(Accel_X,Accel_Z)*180/PI;                 //计算与地面的夹角	
		  Gyro_Y=Gyro_Y/16.4;                                    //陀螺仪量程转换	
      if(Way_Angle==2)		  	Kalman_Filter(Accel_Y,-Gyro_Y);//卡尔曼滤波	
			else if(Way_Angle==3)   Yijielvbo(Accel_Y,-Gyro_Y);    //互补滤波
	    Angle_Balance=angle;                                   //更新平衡倾角
			Gyro_Turn=Gyro_Z;                                      //更新转向角速度
	  	}
}
/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
