#include "stm32f10x.h" 
#include "Delay.h" 
#include "Motor.h" 

#define t_Stop 500
uint32_t Distance;
extern uint32_t Count_TIM1_YiChuZhi;

//Trig:PB15  Echo:PB14
void SR04_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//Trig
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);//置低
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//Echo
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
}
uint32_t Cal_Distance(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_15);//输出高电平
	Delay_us(20);//大于10us即可
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);//置低
	static uint32_t t = 0;//static 函数结束后变量自动销毁
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)== 0){//等待0结束，即碰到上升沿表示回响开始 跳出循环
		TIM_SetCounter(TIM1,0);//便于后续计算
		Count_TIM1_YiChuZhi=0;//溢出次数清零后，定时器1更新中断依旧不断执行，累积溢出次数Count_TIM1_YiChuZhi
	}
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)== 1){//等待1结束，即碰到下降沿表示回响结束 跳出循环计算时间
		t=Count_TIM1_YiChuZhi*1000+TIM_GetCounter(TIM1);
  Distance = t/58;//单位是cm
	}
	Delay_ms(60);//确保测量周期60ms以上 防止发射信号对回响信号的影响
	return Distance;
}
void Obstacle_Avoidance(void){
			Motor_Back_PWM();             //Step1
			Delay_ms(300); //??    0.3?
			Motor_Stop();
			Delay_ms(t_Stop);    //??t_Stop ?
			
			Motor_SelfRight_PWM();        //Step2
			Delay_ms(200); //????0.3?
			Motor_Stop();
			Delay_ms(t_Stop);    //??t_Stop ?
			
			Motor_Forward_PWM();          //Step3
			Delay_ms(1100); //??    0.5?
			Motor_Stop();
			Delay_ms(t_Stop);    //??t_Stop ?
			
      Motor_SelfLeft_PWM();         //Step4
			Delay_ms(410); //????0.4?
			Motor_Stop();
			Delay_ms(t_Stop);    //??t_Stop ?
			
			Motor_Forward_PWM();          //Step5
			Delay_ms(1170); //??    0.5?
			Motor_Stop();
			Delay_ms(t_Stop);    //??t_Stop ?
			
			Motor_SelfRight_PWM();        //Step6
			Delay_ms(220); //????0.3?
			Motor_Stop();
			Delay_ms(t_Stop);    //??t_Stop ?
}

