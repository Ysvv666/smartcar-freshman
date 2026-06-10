#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Bluetooth_RxData;
uint8_t Bluetooth_RxFlag;

// USART3_TX:PB10   USART3_RX:PB11
void Bluetooth_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // ??TX?????????
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ??RX???????
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ??USART3??
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);
    
    // ??????
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    // ??NVIC?????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
    
    // ??USART3
    USART_Cmd(USART3, ENABLE);
}

// ??????
void Bluetooth_SendByte(uint8_t Byte)
{
    USART_SendData(USART3, Byte);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

// ??????
void Bluetooth_SendArray(uint8_t *Array, uint16_t Length)
{
    for (uint16_t i = 0; i < Length; i++) {
        Bluetooth_SendByte(Array[i]);
    }
}

// ?????
void Bluetooth_SendString(char *String)
{
    for (uint8_t i = 0; String[i] != '\0'; i++) {
        Bluetooth_SendByte(String[i]);
    }
}

// ????????
static uint32_t Bluetooth_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--) {
        Result *= X;
    }
    return Result;
}

// ????
void Bluetooth_SendNumber(uint32_t Number, uint8_t Length)
{
    for (uint8_t i = 0; i < Length; i++) {
        Bluetooth_SendByte(Number / Bluetooth_Pow(10, Length - i - 1) % 10 + '0');
    }
}

// ?????(?????fputc??,???????)
// ??:??Serial.c???fputc,?????,??????
/* int fputc(int ch, FILE *f)
{
    Bluetooth_SendByte(ch);
    return ch;
} */

void Bluetooth_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Bluetooth_SendString(String);
}

// ?????????
uint8_t Bluetooth_GetRxFlag(void)
{
    if (Bluetooth_RxFlag) {
        Bluetooth_RxFlag = 0;
        return 1;
    }
    return 0;
}


uint8_t Bluetooth_GetRxData(void)
{
    return Bluetooth_RxData;
}
extern uint8_t Mode;			
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
        Bluetooth_RxData = USART_ReceiveData(USART3);
        Bluetooth_RxFlag = 1;
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}    