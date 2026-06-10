#ifndef __PID_H
#define __PID_H
extern int error;
extern float Out;

int Read_Status(void);
void Calculate_error(void);               
void Position_PID (int Actual,int Target);                       
#endif 
