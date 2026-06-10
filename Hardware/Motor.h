#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor_Left_PWM(float out);
void Motor_Right_PWM(float out);
void Motor_Forward_PWM(void);
void Motor_Back_PWM(void);
void Motor_SelfLeft_PWM(void);
void Motor_SelfRight_PWM(void);
void Motor_Stop(void);

#endif
