#include "stm32f10x.h"                  // Device header
#include "Motor.h"

#define D1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) 
#define D2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) 
#define D3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) 
#define D4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define D5 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)
#define D6 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)

int Read_Status(void){
	int Status=0;
  Status+=D1*100000;
  Status+=D2*10000;
  Status+=D3*1000;
  Status+=D4*100;
  Status+=D5*10;
  Status+=D6;
	return Status;
}

int error;
double Speed_Status=0;

void Calculate_error(void){
	int Current_Status=0;
	Current_Status=Read_Status();
	switch(Current_Status){

		case 111000: error=-11;Speed_Status=1.00;     break;//直角
		case 100000: error=-9; Speed_Status=0.80;     break;
		case 110000: error=-7; Speed_Status=0.85;     break;
		case  10000: error=-5; Speed_Status=0.90;     break;
		case  11000: error=-3; Speed_Status=0.95;     break;
		case   1000: error=-1; Speed_Status=0.95;     break;
		
		case      0: error= 0; Speed_Status=1.00;     break;//当中间两个红外模块距离宽于黑线宽度
		//case 001100: error=0 ; Speed_Status=1.00;     break;//当中间两个红外模块距离短于黑线宽度

		case    100: error= 1; Speed_Status=0.95;     break;
		case    110: error= 3; Speed_Status=0.80;     break;
		case     10: error= 5; Speed_Status=0.85;     break;
		case     11: error= 7; Speed_Status=0.90;     break;
		case     01: error= 9; Speed_Status=0.95;     break;
		case    111: error= 11;Speed_Status=1.00;     break;//直角

		case 111111: error=error; Speed_Status=0;     break;//刹车
//		default:
//            error = 0;
//            Speed_Status = 1;
//            break;
	}
}     
int Out_left,Out_right;
float Kp=20,
				Ki=0,
				Kd=55;
float Bias,Last_Bias,BiasInt , Out;                                      
void Position_PID (int Actual,int Target){ //Actual=error,Target=0;                                         
	Last_Bias=Bias;                                    
  Bias=Actual-Target;   

	if(Ki!= 0){ 
		BiasInt+=Bias;
	}else{
		BiasInt=0;
	}
	
  Out=Kp*Bias+Ki*BiasInt+Kd*(Bias-Last_Bias);	
	
	if (Out > 100) {Out = 100;}
	if (Out <-100) {Out =-100;}
	
	Out_left=70+Out;
	Out_right=76-Out;
	
	if (Out_left > 100) {Out_left = 100;}
	if (Out_left < -100)  {Out_left =-100;}
	if (Out_right> 100) {Out_right = 100;}
	if (Out_right< -100)  {Out_right =-100;}

	Motor_Left_PWM(Out_left*Speed_Status);
	Motor_Right_PWM(Out_right*Speed_Status);
}

