#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
//#define PWM_AMP 7200   //PWM的分辨率
#define NumOfMotor 4

typedef enum 
{
	POS,
	WAVE,
	STOP,
}t_ControlMode;

typedef struct 
{
	t_ControlMode Mode;
	int PWM_out;
	float Pos;
	float A;
	float Offset;
	float Fq;
	float phi;
}STmotorCMD;


int ConfigMotorCMD(STmotorCMD* pMotor, t_ControlMode mode, float POS, float A, float Offset, float Fq,float phi);


extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
int TIM1_UP_IRQHandler(void);  
int balance(float angle,float gyro);
int velocity(int encoder_left,int encoder_right);
int turn(int encoder_left,int encoder_right,float gyro);
void Set_Pwm(int moto1,int moto2,int moto3,int moto4);
void Key(void);
void Xianfu_Pwm(void);
u8 Turn_Off(float angle, int voltage);
void Get_Angle(u8 way);
int myabs(int a);

double waveOut( double A, double fq, double phi,  double offset, int t);
int PWMOut( STmotorCMD* pMotor,int t);

int MyMotorInt(void);

#endif
