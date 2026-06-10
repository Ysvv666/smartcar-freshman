#include "stm32f10x.h"                  // Device header
#include "PWM.h"
// 电机控制使用的是EN1、2置高电平,通过控制IN1、2，3、4来调节电机

/*****经过2025.4.19调试,左轮70 右轮76 此时刚好直线前进*****/

void Motor_Init(void)
{
	PWM_Init(); 
}
//左轮
void Motor_Left_PWM(float out){
	if(out>0){//正转
		TIM_SetCompare1(TIM4,(uint16_t)out);
		TIM_SetCompare2(TIM4,0);
	}else{//反转
		TIM_SetCompare1(TIM4,0);
		TIM_SetCompare2(TIM4,(uint16_t)(-out));
	}
}
//右轮
void Motor_Right_PWM(float out){
	if(out>0){//正转
		TIM_SetCompare3(TIM4,(uint16_t)out);
		TIM_SetCompare4(TIM4,0);
	}else{//反转
		TIM_SetCompare3(TIM4,0);
		TIM_SetCompare4(TIM4,(uint16_t)(-out));
	}
}
//前进
void Motor_Forward_PWM(void){
		//左轮正转
		TIM_SetCompare1(TIM4,70);
		TIM_SetCompare2(TIM4,0);
		//右轮正转
		TIM_SetCompare3(TIM4,76);
		TIM_SetCompare4(TIM4,0);
}
//后退
void Motor_Back_PWM(void){
		//左轮反转
		TIM_SetCompare1(TIM4,0);
		TIM_SetCompare2(TIM4,70);
		//右轮反转
		TIM_SetCompare3(TIM4,0);
		TIM_SetCompare4(TIM4,76);
}
//原地左转
void Motor_SelfLeft_PWM(void){
		//左轮反转
		TIM_SetCompare1(TIM4,0);
		TIM_SetCompare2(TIM4,70);
		//右轮正转
		TIM_SetCompare3(TIM4,76);
		TIM_SetCompare4(TIM4,0);
}
//原地右转
void Motor_SelfRight_PWM(void){
		//左轮正转
		TIM_SetCompare1(TIM4,70);
		TIM_SetCompare2(TIM4,0);
		//右轮反转
		TIM_SetCompare3(TIM4,0);
		TIM_SetCompare4(TIM4,76);
}
void Motor_Stop(void){
		//左
		TIM_SetCompare1(TIM4,0);
		TIM_SetCompare2(TIM4,0);
		//右
		TIM_SetCompare3(TIM4,0);
		TIM_SetCompare4(TIM4,0);
}

//TIM_SetCompareX(TIMX,Compare);Compare的范围是0-ARR;占空比Ducy= (Compare/ARR+1) * 100%

