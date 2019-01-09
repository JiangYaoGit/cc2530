#include "DimmerDrive.h"
#include "iocc2530.h"

void Uart0Init(void)
{
  PERCFG &= ~0x01; //串口0位置1 P02-RX P03-TX
  P0SEL |= 0X0c;
  P2DIR &= ~0XC0;  //串口0优先级最高
  
  U0CSR |= 0Xc0;
  U0GCR = 11;
  U0BAUD = 216;    //115200
  
  UTX0IF = 0;      //UART0 TX中断标志初始置位0
  URX0IF = 0;
  IEN0 |= 0X04;    //使能串口0中断 
  
  T3CTL = 0xa6;   //32分频 计数溢出中断关闭 清除计数值 模模式
  T3IE = 1;       //T3总中断使能
  T3CCTL0 = 0x54; //通道中断使能0 比较模式 比较时切换
  T3CC0 = 200;    //200us中断一次 
  StartT3;
   
  T4CTL = 0xa6;   //32分频 计数溢出中断关闭 清除计数值 模模式
  T4IE = 1;       //T3总中断使能
  T4CCTL1 = 0x54; //通道中断使能1关闭 比较模式 比较时切换
  T4CC0 = 200;    //200us中断一次  
}

uint8 UART0TxBuf[UART0_BUF_SIZE] = {0xFA,0x0A,0x12,0x02,0xFF,0xFF,0xFF,0xFF,
0x02,0x01,0x01,0x00,0x18};

void Uart0Sent(uint8 *address,uint8 len)
{
  char i = 0;
  for(i = 0;i < len;i++)
  {
    U0DBUF = *address++;
    while(UTX0IF == 0);  //发送完成标志
    UTX0IF = 0;
  }
}

/*********************串口0中断处理函数************/
uint8 timeStart=0; 
uint8 UART0RxBuf[UART0_BUF_SIZE] = "";
uint8 UART0RxBufLen = 0;

HAL_ISR_FUNCTION( halUart0Isr, URX0_VECTOR )
{
  EA = 0;  
  URX0IF = 0;    // 清中断标志

  if(UART0RxBufLen < UART0_BUF_SIZE)
  {
    UART0RxBuf[UART0RxBufLen++] = U0DBUF; 
  }
  if( 0 == timeStart )
  {
    StartT4; //T4CTL |   = 0x10;
    timeStart = 1;
  }
  EA = 1;
}

/*********************定时器3中断处理函数************/
static uint16 time3Cnt1 = 0;
static uint16 time3Cnt2 = 0;

HAL_ISR_FUNCTION(halTimer3Isr,T3_VECTOR)
{ 
  time3Cnt1++;
  if(time3Cnt1 >= Time3MaxCnt1)    
  { 
    time3Cnt1  = 0;
    //面板退网申请，反馈事件 200ms
    if(LeaveNWKCnt > 0)
    {
      LeaveNWKCnt--;
      osal_set_event( UserApp_TaskID,NWK_BACK_EVT);
    }
    
    //10s自检网络状态
    time3Cnt2++;   
    if(time3Cnt2 >= Time3MaxCnt2)
    {
      time3Cnt2 = 0;
      if(NWKStateFlag == 0) 
      {
        osal_set_event( UserApp_TaskID,NWK_AUTO_EVT);
      }
    }
  }
}
/*********************定时器4中断处理函数************/
static uint16 time4Cnt = 0;
HAL_ISR_FUNCTION(halTimer4Isr,T4_VECTOR)
{ 
  time4Cnt++;
  if(time4Cnt >= Time4MaxCnt)    
  {
    StopT4;
    time4Cnt  = 0;
    timeStart = 0;
    osal_set_event( UserApp_TaskID,UART0_RX_EVT);
  }
}

//网关下发数据  十进制到十六进制
uint8 DecToHex(uint8 value)
{
  uint8 retValue = 0;
  if(value == 0)
  {
    retValue = 0;
  } 
  else if(value == 100)
  {
    retValue = 254;
  }
  //偶数 value/2*2 + (value/2-1)*3 + 2
  else if(value % 2 == 0) 
  {
    retValue = value +(value / 2 - 1)*3 + 2;
  }
  //奇数(uint8)(value/2)*2 + (uint8)(value/2)*3 + 2
  else if(value % 2 == 1 )
  {
    value = (uint8)(value / 2);
    retValue = value * 5 + 2;
  }
  return retValue;
}


//设备上传 十六进制到十进制
uint8 HexToDec(uint8 value)
{
  uint8 retValue = 0;
  uint8 var = 0;
  
  if(value == 0)
  {
    retValue = 0;
  }
  else if(value > 247)
  {
    retValue = 100;
  }
  /***将1-100每两个数看出一个数，对应50个数组***/
  else
  { 
    //取出对应的数组下标和数组内的对应值
    if(value % 5 == 0)
    {
      var = 5;
      value = value/5 - 1; 
    }
    else if(value % 5 != 0)
    {
      var = value % 5;
      value = value / 5;
    }
    
    //计算十进制retValue值
    if(var > 2)
    {
      retValue = value*2 + 2;
    }
    else 
    {
      retValue = value*2 + 1;
    } 
  }

  return retValue; 
}
