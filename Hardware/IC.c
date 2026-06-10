#include "stm32f10x.h"                  // Device header

/**
  * 函    数：输入捕获初始化
  * 参    数：无
  * 返 回 值：无
  */
	           //定时器1 通道3   //Trig:PB15  Echo:输入捕获PB14
void IC_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//开启GPIOB的时钟
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);//下拉

}
