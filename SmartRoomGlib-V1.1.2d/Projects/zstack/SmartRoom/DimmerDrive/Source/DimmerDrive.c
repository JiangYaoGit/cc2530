#include "DimmerDrive.h"
#include "iocc2530.h"

void Uart0Init(void)
{
  PERCFG &= ~0x01; //����0λ��1 P02-RX P03-TX
  P0SEL |= 0X0c;
  P2DIR &= ~0XC0;  //����0���ȼ����
  
  U0CSR |= 0Xc0;
  U0GCR = 11;
  U0BAUD = 216;    //115200
  
  UTX0IF = 0;      //UART0 TX�жϱ�־��ʼ��λ0
  URX0IF = 0;
  IEN0 |= 0X04;    //ʹ�ܴ���0�ж� 
  
  T3CTL = 0xa6;   //32��Ƶ ��������жϹر� �������ֵ ģģʽ
  T3IE = 1;       //T3���ж�ʹ��
  T3CCTL0 = 0x54; //ͨ���ж�ʹ��0 �Ƚ�ģʽ �Ƚ�ʱ�л�
  T3CC0 = 200;    //200us�ж�һ�� 
  StartT3;
   
  T4CTL = 0xa6;   //32��Ƶ ��������жϹر� �������ֵ ģģʽ
  T4IE = 1;       //T3���ж�ʹ��
  T4CCTL1 = 0x54; //ͨ���ж�ʹ��1�ر� �Ƚ�ģʽ �Ƚ�ʱ�л�
  T4CC0 = 200;    //200us�ж�һ��  
}

uint8 UART0TxBuf[UART0_BUF_SIZE] = {0xFA,0x0A,0x12,0x02,0xFF,0xFF,0xFF,0xFF,
0x02,0x01,0x01,0x00,0x18};

void Uart0Sent(uint8 *address,uint8 len)
{
  char i = 0;
  for(i = 0;i < len;i++)
  {
    U0DBUF = *address++;
    while(UTX0IF == 0);  //������ɱ�־
    UTX0IF = 0;
  }
}

/*********************����0�жϴ�����************/
uint8 timeStart=0; 
uint8 UART0RxBuf[UART0_BUF_SIZE] = "";
uint8 UART0RxBufLen = 0;

HAL_ISR_FUNCTION( halUart0Isr, URX0_VECTOR )
{
  EA = 0;  
  URX0IF = 0;    // ���жϱ�־

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

/*********************��ʱ��3�жϴ�����************/
static uint16 time3Cnt1 = 0;
static uint16 time3Cnt2 = 0;

HAL_ISR_FUNCTION(halTimer3Isr,T3_VECTOR)
{ 
  time3Cnt1++;
  if(time3Cnt1 >= Time3MaxCnt1)    
  { 
    time3Cnt1  = 0;
    //����������룬�����¼� 200ms
    if(LeaveNWKCnt > 0)
    {
      LeaveNWKCnt--;
      osal_set_event( UserApp_TaskID,NWK_BACK_EVT);
    }
    
    //10s�Լ�����״̬
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
/*********************��ʱ��4�жϴ�����************/
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

//�����·�����  ʮ���Ƶ�ʮ������
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
  //ż�� value/2*2 + (value/2-1)*3 + 2
  else if(value % 2 == 0) 
  {
    retValue = value +(value / 2 - 1)*3 + 2;
  }
  //����(uint8)(value/2)*2 + (uint8)(value/2)*3 + 2
  else if(value % 2 == 1 )
  {
    value = (uint8)(value / 2);
    retValue = value * 5 + 2;
  }
  return retValue;
}


//�豸�ϴ� ʮ�����Ƶ�ʮ����
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
  /***��1-100ÿ����������һ��������Ӧ50������***/
  else
  { 
    //ȡ����Ӧ�������±�������ڵĶ�Ӧֵ
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
    
    //����ʮ����retValueֵ
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
