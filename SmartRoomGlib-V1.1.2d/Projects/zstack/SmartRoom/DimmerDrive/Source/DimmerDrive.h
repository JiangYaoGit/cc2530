#ifndef __DIMMER_DRIVE_H
#define __DIMMER_DRIVE_H

#include "AF.h"
#include "OnBoard.h"
#include "ZDObject.h"
#include "hal_types.h"
#include "wzcl.h"
#include "Deviceconfig.h"

#ifdef __cplusplus
extern "C" {
#endif
  
#define Led P1_0
#define Out P1_6
  
#define UART0_BUF_SIZE 60     
#define Time3MaxCnt1 1000        //0.2ms*Time3MaxCnt1
#define Time3MaxCnt2 50         //200ms*Time3MaxCnt2
#define Time4MaxCnt 250         //0.2ms*Time4MaxCnt

#define StartT3     T3CTL |= 0x10 
#define StopT3      T3CTL &= ~0x10   
#define StartT4     T4CTL |= 0x10 
#define StopT4      T4CTL &= ~0x10 

extern uint8 LeaveNWKCnt;          //退网反馈计数
extern volatile uint8 NWKStateFlag;         //网络状态标志位
  
extern uint8 UART0RxBuf[UART0_BUF_SIZE];
extern uint8 UART0RxBufLen;
extern uint8 UART0TxBuf[UART0_BUF_SIZE];

void Uart0Init(void);
void Uart0Sent(uint8 *address,uint8 len);

//格式转换
uint8 DecToHex(uint8 value);
uint8 HexToDec(uint8 value);
  
#ifdef __cplusplus
}
#endif

#endif