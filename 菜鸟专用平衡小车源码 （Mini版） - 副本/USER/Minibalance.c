#include "sys.h"
#include "control.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PWM_FQ 10000

u8 Way_Angle=2;                             //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波 默认搭载卡尔曼滤波
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //蓝牙遥控相关的变量
u8 Flag_Stop=0,Flag_Show=1;                 //停止标志位和 显示标志位 默认停止 显示打开
int Encoder_Left,Encoder_Right;             //左右编码器的脉冲计数
int Moto1,Moto2;                            //电机PWM变量 应是Motor的 向Moto致敬	
int Temperature;                            //显示温度
int Voltage;                                //电池电压采样相关的变量
float Angle_Balance,Gyro_Balance,Gyro_Turn; //平衡倾角 平衡陀螺仪 转向陀螺仪
float Show_Data_Mb;                         //全局显示变量，用于显示需要查看的数据

const int PWM_AMP = 7200 ;

int main(void)
{ 
	//NVIC_SetVectorTable();
	Stm32_Clock_Init(9);            //系统时钟设置
	delay_init(72);                 //延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);     //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //=====打开SWD接口 可以利用主板的SWD接口调试
	LED_Init();                     //初始化与 LED 连接的硬件接口
	KEY_Init();                     //按键初始化
	//OLED_Init();                    //OLED初始化
	uart_init(72,115200);           //初始化串口1
  uart2_init(36,9600);            //串口2初始化
	MiniBalance_PWM_Init(PWM_AMP-1,30-1);   //=====初始化PWM ，用于驱动电机  频率72M/(7199+1)/30 = 333Hz 
//	Encoder_Init_TIM2();            //=====编码器接口
//	Encoder_Init_TIM3();            //初始化编码器2 
	IIC_Init();                     //模拟IIC初始化
  MPU6050_initialize();           //=====MPU6050初始化	
	DMP_Init();                     //初始化DMP    
  Timer1_Init(49,7199);           //=====5MS进一次中断服务函数
	
	MyMotorInt();  // 初始化舵机的控制参数
	while(1)
		{
       
					Temperature=Read_Temperature();  //===读取MPU6050内置温度传感器数据，近似表示主板温度。	
				//	oled_show(); //===显示屏打开
				//  printf("平衡倾角%f  左轮编码器%d  右轮编码器%d  电池电压%dmV\r\n",Angle_Balance,Encoder_Left,Encoder_Right,Voltage);//向上位机发送数据
					delay_ms(100);	//延时减缓数据传输频率，确保通信的稳定

		} 
}
