#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "Irtracking.h"
#include "Motor.h"
#include "Timer.h"
#include "PID.h"
#include "LED.h"
#include "OLED.h"
#include "Serial.h"
#include "HC_SR04.h"
#include "Bluetooth.h"
#include "Servo.h"

volatile uint8_t Vehicle_State=0;//0:正常循迹状态 1:超声波避障状态
#define Distance_Obstacle 9//定义超声波测距最小值  单位(cm)
extern uint32_t Distance;
//volatile 可被外部因素修改；extern 变量在各个文件中共享
uint32_t Count_TIM1_YiChuZhi=0;//用于记住定时器1的溢出值 计算超声波测距的时间
uint8_t RxData;			//定义用于接收蓝牙串口数据的变量
uint8_t Mode=1;			//小车自动控制

/*这里只是为了让我这个小菜鸡记住各种类型*/
//#define int8_t  signed char
//#define int16_t signed short
//#define int32_t signed int
//#define int64_t signed long long int
//#define uint8_t  unsigned char
//#define uint16_t unsigned short
//#define uint32_t unsigned int
//#define uint64_t unsigned long long int

int main(void)
{	
	SR04_Init();//5V 2cm~600cm 精度3mm
	OLED_Init();
	LED_Init();
	Motor_Init();        
	Irtracking_Init();//循迹红外传感器初始化  
	Timer1_Init();//定时器1用于PID循迹和超声波测距计算时间时的 溢出值记忆Count_TIM1_YiChuZhi
	Serial_Init();
	Bluetooth_Init();
  LED1_ON();	
	Delay_ms(500);
	LED1_OFF();
	Servo_Init();    //舵机初始化
	while (1)
	{
		if(Mode==1){//小车自动控制
			Distance = Cal_Distance();
			if(Distance <= Distance_Obstacle){ //避障状态
				Vehicle_State=1; // 设置为避障状态
				//OLED显示超声波避障进行中
				OLED_Clear();
				OLED_Printf(0, 0, OLED_8X16, "HC-SR04");
				OLED_Printf(0, 16, OLED_8X16, "Obstacle:%d", Distance_Obstacle);
				OLED_Printf(0, 32, OLED_8X16, "Distance:%+04d",Distance);
				OLED_Update();
				//蓝牙发送超声波检测距离数据
				Bluetooth_Printf("%f\r\n",Distance);		
				// 遇到障碍，执行绕过障碍的逻辑
				Obstacle_Avoidance();   
				Vehicle_State = 0; // 恢复循迹状态 循迹中断继续执行
			}else{ 
				OLED_Clear();
			//OLED显示PID循迹参数	
				OLED_Printf(0, 0, OLED_8X16, "PID Tracking:");
				OLED_Printf(0, 16, OLED_8X16, "Target:%d", 0);
				OLED_Printf(0, 32, OLED_8X16, "Actual:%+02d", error);
				OLED_Printf(0, 48, OLED_8X16, "Out:%+03.0f", Out);	
				OLED_Update();
			//串口收发PID循迹数据
				Serial_Printf("%d,%d,%f\r\n",0, error,Out);
			}
		}
		if(Bluetooth_GetRxFlag()==1)			//检查蓝牙串口接收数据的标志位 同时函数自动清除 Bluetooth_RxFlag
		{	
			OLED_Clear();
			Mode=2;//手动控制小车
			RxData = Bluetooth_GetRxData();		//获取蓝牙串口接收的数据
			OLED_Printf(0, 0, OLED_8X16, "Bluetooth Task:");
			LED1_ON();	
			if (RxData=='A'){
				//前进
				Motor_Left_PWM(80);
				Motor_Right_PWM(80);
				OLED_Printf(0, 16, OLED_8X16, "Run");
			}
			if (RxData=='B'){
				//后退
				Motor_Left_PWM(-80);
				Motor_Right_PWM(-80);
				OLED_Printf(0, 16, OLED_8X16, "Back");

			}
			if (RxData=='C'){
				//左转
				Motor_Left_PWM(-60);
				Motor_Right_PWM(60);
				OLED_Printf(0, 16, OLED_8X16, "Left");
			}
			if (RxData=='D'){
				//右转
				Motor_Left_PWM(60);
				Motor_Right_PWM(-60);
				OLED_Printf(0, 16, OLED_8X16, "Right");
			}
			if (RxData=='E'){
				//检测前方距离
				Distance = Cal_Distance();
				OLED_Printf(0, 16, OLED_8X16, "HC-SR04");
		  	OLED_Printf(0, 32, OLED_8X16, "Distance:%+04d",Distance);
			  Bluetooth_Printf("%04d\r\n",Distance);
			}
			if (RxData=='F'){
			  Bluetooth_Printf("Hello!\r\n");
				OLED_Clear();
				OLED_Printf(24, 16, OLED_8X16, "|");
				OLED_Printf(24, 32, OLED_8X16, "|");
				OLED_Printf(96, 16, OLED_8X16, "|");
				OLED_Printf(96, 32, OLED_8X16, "|");
				OLED_Printf(40, 20, OLED_8X16, "^");
		  	OLED_Printf(80, 20, OLED_8X16, "^");
				OLED_Printf(60, 32, OLED_8X16, "-");
				OLED_Update();	//OLED显示		
        //摇尾巴 Angle 0~180
				Servo_SetAngle(0);
				Delay_ms(500);
				Servo_SetAngle(180);
				Delay_ms(500);
				Servo_SetAngle(0);
				Delay_ms(500);
				Servo_SetAngle(180);
				Delay_ms(500);
			}
			OLED_Update();	//OLED显示		
			Delay_ms(100);
			//每次接受到数据的时候延迟100ms 
			//并在发送数据时控制长按发送间隔为*****90ms*****
			//确保控制小车时标志位一直处于1的状态
			
		}else if(Bluetooth_GetRxFlag()==0 && Mode==2){//当接收标志为为0时，并且此时处于手动控制结束状态 即Mode==2
                                                  //没有发送数据  小车自动停止
					//这意味着需要 长按不断发送数据 使标志位一直处于1的状态 才能实现控制小车运动
					//一但松开按键 小车停止 LED灯灭 更改为模式1状态
			Motor_Left_PWM(0);
			Motor_Right_PWM(0);				
			LED1_OFF();
			Mode=1;
		}
	}
}
/*定时器1更新中断函数*/
/*抢占优先级高于蓝牙的USART3接收中断*/
/*这是因为Count_TIM1_YiChuZhi++这个时序不能停 这样才能准确地测距*/
void TIM1_UP_IRQHandler(void){  //每毫秒进一次
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET){
		if(Vehicle_State==0 && Mode==1){//Vehicle_State==0表示超声波检测正常 未遇到障碍 此时为避障模式
			Calculate_error();            //Mode=1表示小车自动控制模式 
			Position_PID(error,0);
		}
		Count_TIM1_YiChuZhi++;//这里是定时器1的溢出值 用于计算接超声波回响的时间
	 }
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}
	