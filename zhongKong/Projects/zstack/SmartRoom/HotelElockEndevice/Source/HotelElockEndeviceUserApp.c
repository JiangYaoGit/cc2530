/**
******************************************************************************
* @file    SampleDeviceUserApp.c
* @author  Wireless Protocol Stack Develop-Dept.
* @version 1.2.0
* @date    2016/12/12,yuzz
* @brief  Sample Device Application (no Profile).
*
* @verbatim
*
******************************************************************************
* @attention
*    modified by yuzz   @2016/12/12   E-mail: zhengzheng.yu@wuliangroup.com
*
*
* <h2><center>&copy; COPYRIGHT 2016 WuLianGroup</center></h2>
******************************************************************************
*/
/*******************************************************************************
* INCLUDES
*/
#include "AF.h"
#include "OnBoard.h"
#include "ZDObject.h"
#include "hal_led.h"
#include "hal_key.h"
#include "DeviceConfig.h"

#include "ZDObject.h"
#include "OSAL_PwrMgr.h"
#include "UserUart.h"
/*******************************************************************************
* LOCAL CONSTANTS
*/

/*******************************************************************************
* LOCAL FUNCTIONS
*/
static void UserAppProcessKeyNwkAction(void);
static void UserAppHandleKeys(uint8 shift, uint16 keys );
static void UserAppProcessZDOMsgs(zdoIncomingMsg_t *inMsg);

void WakeLowLevel(void);

uint8 UART_SEND_DOWN[50];    //ģ���յ���λ�����ݺ󣬸��豸���͵�����
uint8 LEN_UART_SEND_DOWN;   //�������ݵĳ���

uint8  dataReportUp[60];
uint16 dataLenReportUp;   

extern uint8  UART0_RxBuf[600];

uint16 UartCopyLength = 0;
uint16 Custer_num_total =0;//�ͻ������ۼ�

uint8  wireless_commnd_flag = 0;//����������λ 
uint8  Pkg_flag;//�����ְ���־
uint8  Pkg_cnt,Pkg_cnt_Mod;
uint8  FenBao_cnt=0;
uint8  Pkg_cnt_flag=0;

uint8 dataReportUp_flag=0;
uint8 ResponseCount = 0; //Ӧ�����
uint8 CheckCode     = 0; //����У��
uint8 ResponseError =0 ;
uint8 Leave_NET_flag=0;

CasualUserData_t  CasualUserData;
CommonUserData_t  CommonUserData;
CommonUserEx_t    CommonUserEx;

uint16 CommonUserBuffEx;
uint16 CasualUserBuffEx;

uint16 PasswordLen;
uint8  *Password;
uint8  *PasswordEx;

uint8 SceneSwitch_Mes_Write[1]={0};
uint8 SceneSwitch_Mes_Read[1]={0};

uint8 SceneSwitch_Count_Write[1]={0};
uint8 SceneSwitch_Count_Read[1]={0};

uint8 Commnd_JoinNet_Flag=0;

#define OVER_TIME     500
/*******************************************************************************/
void  UART_CONFIRM_TRUE(void);
void  UART_CONFIRM_ERROR(void);
void  SampleApp_Send_Message(uint8 Value);
void  SampleApp_Send_Message_Ex(uint8 Value);
void  SampleApp_Message(uint8 Value);
void  SampleApp_SendDeviceState(uint16 sta);
void  SampleApp_SendInMessage(void);
void  FenBao_Reportup(void);
void  Delay_device_1u(uint32 microSecs);

uint8  ProcessUartCMD( uint8 *datBuf ,uint16 length);
void Uart_IO_Init(void);
/*******************************************************************************
* GLOBAL VARIABLES
*/
const uint8 FWVersion   = 0x0A;
const uint8 FWDate[] = (__DATE__);
const uint8 DisableManufacture = FALSE;
const uint8 License[10] = {0x64,0x27,0xC6,0xA8,0x66,0xFC,0x10,0x07,0x1C,0x1E};
/*******************************************************************************
* LOCAL VARIABLES
*/
#define USERAPP_IN_BINDINGLIST          1
const cId_t bindingInClusterList[USERAPP_IN_BINDINGLIST]=
{
  USERAPP_CLUSTER_ID_GEN_ONOFF
};
#define USERAPP_OUT_BINDINGLIST         1
const cId_t bindingOutClusterList[USERAPP_OUT_BINDINGLIST]=
{
  USERAPP_CLUSTER_ID_GEN_ONOFF
};
uint8 UserApp_TaskID;

bool BlinkLedForRssi = FALSE;
/*******************************************************************************
* @fn      Respond_Error
* @brief   �����Ӧ
* @param   none
* @return  none
*******************************************************************************/
void UART_CONFIRM_ERROR(void)
{
  UART_SEND_DOWN[0] = 0xA1;
  UART_SEND_DOWN[1] = 0x01;
  UART_SEND_DOWN[2] = 0x00;
  UART_SEND_DOWN[3] = 0x01;
  UART_SEND_DOWN[4] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+ UART_SEND_DOWN[3];
  LEN_UART_SEND_DOWN = 5;
  
  //WakeLowLevel();
  osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,10);
}
/*******************************************************************************
* @fn      Respond_Error
* @brief   ��ȷ��Ӧ
* @param   none
* @return  none
*******************************************************************************/
void UART_CONFIRM_TRUE(void)
{
  wireless_commnd_flag=0;
  
  UART_SEND_DOWN[0] = 0xA1;
  UART_SEND_DOWN[1] = 0x00;
  UART_SEND_DOWN[2] = 0x00;
  UART_SEND_DOWN[3] = 0x01;
  UART_SEND_DOWN[4] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
  LEN_UART_SEND_DOWN = 5;
  
  //WakeLowLevel();
  osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,10);
}
/*******************************************************************************
* @fn      ProcessUartCMD
* @brief   ����������
* @param   uint8 *datBuf ,uint8 length
* @return  none
*/
uint8 ProcessUartCMD( uint8 *datbuf ,uint16 length)
{
  uint16 i;
  uint8 Check=0;
  
  if(datbuf[0]==0xA1)     //��ͷ��ȷ
  {
    UART_SEND_DOWN[0]=0xA1;
    for(i=0;i<length-1;i++)
    {
      Check += datbuf[i];  //����У��ֵ 
    }
    //    if(Check != datbuf[length-1])  //����У�����
    //    {
    //      UART_CONFIRM_ERROR();
    //      return 0;
    //    }
    UART_SEND_DOWN[0] = 0xA1;
    osal_stop_timerEx (UserApp_TaskID,OVERTIME_EVT);
    switch(datbuf[1])
    {
      /****************************ȷ����ȷ************************************/ 
    case 0x00:  
      {
        ResponseError =0; 
        wireless_commnd_flag=0; 
      }break;      
      /**************************У����߷��ش���******************************/    
    case 0x01:  
      {
        CheckCode = 1;
        ResponseError++; 
        if(ResponseError<3) //С��3����Ҫ�ٴν��з���
        {
          //WakeLowLevel();
          osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,10);
        }
        else
        {
          ResponseError = 0;
          CheckCode = 0;
          
          switch(wireless_commnd_flag)
          {
          case 0x11:      //������֤APP����ʧ
            SampleApp_SendDeviceState(0x0A01);     
            break;
            
          case 0x12:      //ʱ��У׼+��̬����ʧ��
            SampleApp_SendDeviceState(0x0A02);     
            break;
            
          case 0x15:      //����Ա��֤ʧ��
            SampleApp_SendDeviceState(0x0A00);     
            break;
            
          case 0x16:      //��̬�����·�ʧ��
            SampleApp_SendDeviceState(0x0A07);     
            break;
            
          case 0x21:     //������û�ʧ��
            SampleApp_SendDeviceState(0x0A03);     
            break;
            
          case 0x22:     //����ɾ��ʧ��
            SampleApp_SendDeviceState(0x0A04);     
            break;
            
          case 0x23:     //�޸�����ʧ��
            SampleApp_SendDeviceState(0x0A05);     
            break;
            
          case 0x24:    //һ�����ʧ��
            SampleApp_SendDeviceState(0x0A06);     
            break;
            
          default:
            break;
          }
          return 0;
        }
      }break;
     /************����У�������Թ���Ա������֤+APP����������֤��***********/       
    case 0x02:
      {
        UART_CONFIRM_TRUE();
        switch(wireless_commnd_flag)
        {          
        case 0x15:     //����Ա������� 
          {
            SampleApp_SendDeviceState(0x0A11);
          }     
          break;
          
        default:
          break;
        }
      }break;
      /*************APP�������--/�����ظ����ز�����/���ʧ��/�������*********/       
    case 0x03:
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A12); 
      }break;
       
    case 0x09:
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A03); 
      }break;
       
    case 0x26:
      {
        UART_CONFIRM_TRUE();
        switch(datbuf[4])
        {
        case 0x01:
          SampleApp_SendDeviceState(0x0A20); //��ͨ�û��������
          break;
          
        case 0x02:
          SampleApp_SendDeviceState(0x0A21); //��ʱ�û��������
          break;
          
        default:
          break;
        }
      }break;
      /************************ɾ���û�ʧ��/�޸�����ʧ��/APP����ʧ��********************************/
    case 0x0A://ɾ���û�ʧ��
      {
        UART_CONFIRM_TRUE(); 
        SampleApp_SendDeviceState(0x0A04); 
      }break;
      
    case 0x0B: //�޸�����ʧ��
      {
        UART_CONFIRM_TRUE(); 
        SampleApp_SendDeviceState(0x0A05);
      }break; 
      
    case 0x0D:  //APP����ʧ��
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A01);
      }break;
      /************************������ʱ********************************/
    case 0x04:  //������ʱ�ɹ�
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0103);
      }break;
      
    case 0x08:  //������ʱʧ��
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A02); 
      }break;
      /************************����Ա��֤********************************/  
    case 0x05:  //��֤�ɹ�
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0105); 
      }break;
      
    case 0x07:  //��֤ʧ��
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A00); 
      }break;
      /*************************һ�����********************************/
    case 0x06: //һ������ɹ�
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0224);   
      }break;
      
    case 0x0C: //һ�����ʧ��
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A06);   
      }break; 
      /**************************���ֲ���������Ϣ�ϱ�***********************/       
    case 0x20:       //������֤ʧ�ܣ�ϵͳ����
    case 0x23:       //��������״̬�ϱ�
    case 0x25:        //�û��������
    case 0x30:       //���Ƿѹ״̬�ϱ�
    case 0x31:       //���ڱ���״̬�ϱ�
    case 0x32:       //������ڱ���
    case 0x36:       //�������ݸ�ʽ���ϱ�
    case 0x3E:       //�������������Ϣ�ϱ�
      {
        if(datbuf[3] == 0x02)
        {
          UART_CONFIRM_TRUE();
          SampleApp_Message(datbuf[1]);//����״̬�ϱ�
        }
        else if(datbuf[3]!=0x01) 
        {
          UART_CONFIRM_ERROR();
        }
        else
        {
          UART_CONFIRM_TRUE();
          SampleApp_Message(datbuf[1]);//����״̬�ϱ�
        }
      }break;
      /*****************************����ָ��*******************************/     
    case 0x40:  
      {
        if((datbuf[2]==0x00)&&(datbuf[3]==0x01))
        {
          UART_CONFIRM_TRUE();
          osal_start_timerEx(UserApp_TaskID,Start_Joinnet_EVT,1000);        
        }
        else
        {
          UART_CONFIRM_ERROR();
        } 
      }break;
      /*****************************��������********************************/     
    case 0x41:   
      {
        if((datbuf[2]==0x00)&&(datbuf[3]==0x01))
        {
          UART_CONFIRM_TRUE();
          
          Leave_NET_flag=1;
          osal_start_timerEx(UserApp_TaskID,NET_Message_EVT,200);//2s��Ӧ���д�������
          osal_start_timerEx(UserApp_TaskID,Leave_Network_EVT,1500);//2s��Ӧ���д�������
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
      }break; 
      /************************���ִ������ϱ�**********************************/
    case 0x3B:                           //��ѯ����״̬�ϱ�
    case 0x24:                          //�����ָ�Ʒ��ٳ�
    case 0x33:                          //����������û���Ϣ
    case 0x34:                           //����ɾ���û���Ϣ
    case 0x35:                           //�����޸Ŀͻ���Ϣ
    case 0x37:                           //�Զ�ɾ����ʱ�û���Ϣ
    case 0x80:                           //���߷���״̬�ϱ�
      {
        if(datbuf[3]!=0x00) //LEN��Ϊ0 
        {
          UART_CONFIRM_TRUE();
          for(i=0;i<datbuf[3]-1;i++) 
          {
            dataReportUp[i]= datbuf[i+4];
          }
          dataLenReportUp = datbuf[3]-1;
          SampleApp_Send_Message(datbuf[1]);
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
        return 1;
      }break;
      /********************************��ֹ/����Զ�̲���**********************/
    case 0x60:
      {
        if(datbuf[3]!=0x00) //LEN��Ϊ0 
        {
          UART_CONFIRM_TRUE();
          if(datbuf[4] == 0x01)
          {
            SampleApp_SendDeviceState(0x0301); 
          }
          else 
          {
            SampleApp_SendDeviceState(0x0300); 
          }
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
      }break;
      /********************************����**********************/
    case 0x61:
      {
        if(datbuf[3]!=0x00) //LEN��Ϊ0 
        {
          UART_CONFIRM_TRUE();
          SampleApp_SendDeviceState(0x0302); 
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
      }break;
      /*************************������Ϣ�ϱ�***********************************/
    case 0x3D:
      {
        if(datbuf[3]!=0x00) //LEN��Ϊ0 
        {
          UART_CONFIRM_TRUE();
          if(datbuf[3]==0x0C)
          {
            for(i=0;i<datbuf[3]-1-8;i++) 
            {
              dataReportUp[i]= datbuf[i+4];
            }
            dataLenReportUp = datbuf[3]-1-7;
          }
          else
          {
            for(i=0;i<datbuf[3]-1;i++) 
            {
              dataReportUp[i]= datbuf[i+4];
            }
            dataLenReportUp = datbuf[3]-1;          
          }
          SampleApp_Send_Message(datbuf[1]);
          osal_start_timerEx(UserApp_TaskID,Auto_Lock_EVT,5000);//2s��Ӧ���д�������
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
        return 1;
      }break;
      /************************************************************************/
    case 0x50:                          //�ش��¼��û���ID
      {
        if(datbuf[3]!=0x00) //LEN��Ϊ0 
        {
          UART_CONFIRM_TRUE();
          
          for(i=0;i<datbuf[3]-1;i++) 
          {
            dataReportUp[i]= datbuf[i+5];
          }
          dataLenReportUp = datbuf[3]-2;
          SampleApp_Send_Message(datbuf[4]+0x50);
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
        return 1;
      }break;
      /************************************************************************/ 
    case 0x38:
      {
        UART_CONFIRM_TRUE();
        
        Custer_num_total = datbuf[2]<<8|datbuf[3];
        dataReportUp[0] = 0x08; //��ʾ�ְ������ϴ�
        dataReportUp[1] = 0x05; //��ʾ�ְ������ϴ�
        
        dataReportUp_flag = 0x03;
        Pkg_cnt = datbuf[4]/2;  //�ְ��İ���
        if(datbuf[4]%2 !=0)
        {
          Pkg_cnt = Pkg_cnt+1; 
          Pkg_cnt_flag = 1;      //δ����
        }
        osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,300); //ÿ300ms �ְ��ϴ�
      }break;
      /************************************************************************/
    case 0x39:                           //��ͨ�û���Ϣ�ϴ�
    case 0x3A:                           //��ʱ�û���Ϣ�ϴ�
      {
        Custer_num_total = datbuf[2]<<8|datbuf[3];
        if(Custer_num_total != 0) //LEN��Ϊ0
        {
          UART_CONFIRM_TRUE();
          switch(datbuf[1])
          {
          case 0x39:
            {
              dataReportUp[0] = 0x08; //��ʾ�ְ������ϴ�
              dataReportUp[1] = 0x06; //��ʾ�ְ������ϴ�
              
              if(datbuf[4] == 0xEE)
              {
                dataReportUp[2] = 0x01; 
                dataReportUp[3] = 0x01;
                dataReportUp[4] = 0xEE; 
                WZCL_SendDsMessage((uint8*)&dataReportUp[0],5);
              }
              else
              {
                dataReportUp_flag = 0x01;
                Pkg_cnt = datbuf[4]/2;  //�ְ��İ���
                if(datbuf[4]%2 !=0)
                {
                  Pkg_cnt = Pkg_cnt+1; 
                  Pkg_cnt_flag = 1;      //δ����
                }
                osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,300); //ÿ300ms �ְ��ϴ�
              }
            }break;
            /*----------------------------------------------------------------*/                      
          case 0x3A:
            {
              osal_stop_timerEx(UserApp_TaskID,SET_POLLTIME_EVT);
              if(devState == DEV_END_DEVICE)
             {
                NLME_SetPollRate(4000);
              }
              
              dataReportUp[0] = 0x08; //��ʾ�ְ������ϴ� 
              dataReportUp[1] = 0x07; //��ʾ�ְ������ϴ�
              
              if(datbuf[4] == 0xEE)
              {
                dataReportUp[2] = 0x01; 
                dataReportUp[3] = 0x01;
                dataReportUp[4] = 0xEE; 
                WZCL_SendDsMessage((uint8*)&dataReportUp[0],5);
              }
              else
              {
                dataReportUp_flag = 0x02; 
                Pkg_cnt = UART0_RxBuf[4];
                osal_start_timerEx(UserApp_TaskID,FenBao_Report_EVT,300); //ÿ300ms �ְ��ϴ�
              }
            }break;
            
          default:
            break;
          } 
        }
        else
        {
          UART_CONFIRM_ERROR(); 
        }
      }break;
      /********************************************************************************/
    case 0x70:       //�������ָ��ID���ϴ�
    case 0x71:       //������ӿ�ƬID���ϴ�
    case 0x72:       //�����ϴ�ָ��ID���ϴ�
    case 0x73:       //�����ϴ���ƬID���ϴ�
      {
        if(datbuf[3]!=0x00) //LEN��Ϊ0 
        {
          UART_CONFIRM_TRUE();
          for(i=0;i<datbuf[3]-1;i++) 
          {
            dataReportUp[i]= datbuf[i+4];
          }
          dataLenReportUp = datbuf[3]-1;
          SampleApp_Send_Message_Ex(datbuf[1]);
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
      }break;
     /********************************************************************************/  
    default:
      break;
    }
  }
  datbuf[1]=0;
  length=0;
  UartCopyLength = 0;
  osal_pwrmgr_device(PWRMGR_BATTERY); 
  return 0;
}
/*******************************************************************************
* USER VARIABLES
*/
/*******************************************************************************
* @fn      UserApp_Init
*
* @brief   Initialization function for the User App Task.
*          This is called during initialization and should contain
*          any application specific initialization (ie. hardware
*          initialization/setup, table initialization, power up
*          notificaiton ... ).
*
* @param   task_id - the ID assigned by OSAL.  This ID should be
*                    used to send messages and set timers.
*
* @return  none
*/
void UserApp_Init( uint8 task_id )
{
  UserApp_TaskID = task_id;
  RegisterForKeys(task_id);
  ZDO_RegisterForZDOMsg(UserApp_TaskID, End_Device_Bind_rsp);
  
  //hal_USART0Init();
  Uart_IO_Init();
}

/*******************************************************************************
* @fn      Uart_IO_Init
*
* @brief   �����ڳ�ʼ��Ϊ��ͨIO�ڣ����룬3̬
*
* @param   none
*
* @return  none
*******************************************************************************/
void Uart_IO_Init(void)
{
  P0SEL &=~0x0C;
  P0DIR &=~0x0C;
  P0INP |=0x0C;
}
/*******************************************************************************
* @fn      UserApp_ProcessEvent
*
* @brief   User Application Task event processor.  This function
*          is called to process all events for the task.  Events
*          include timers, messages and any other user defined events.
*
* @param   task_id  - The OSAL assigned task ID.
* @param   events - events to process.  This is a bit map and can
*                   contain more than one event.
*
* @return  none
*/
uint16 UserApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( UserApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
      case ZDO_CB_MSG:
        UserAppProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
        break;
      case KEY_CHANGE:
        UserAppHandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
        break;
        
      default:
        break;
      }
      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( UserApp_TaskID );
    }//end�� while ( MSGpkt )
    
    return (events ^ SYS_EVENT_MSG);
  }
#if defined(LARGER_NETWORK)
  if ( events & USERAPP_COMMAND_SEND_RSSI_MSG_EVT )
  {
    WZCL_InterRssiReqMessage();
    osal_start_timerEx( UserApp_TaskID,USERAPP_COMMAND_SEND_RSSI_MSG_EVT,3600000);
    return ( events ^ USERAPP_COMMAND_SEND_RSSI_MSG_EVT);
  }
#endif
  /*��-------------------------------------------------------------------------*/  
  if ( events & UART_RX_EVT )
  {
    UartCopyLength = (UART0_RxBuf[2]<<8|UART0_RxBuf[3])+4;
    if(UartCopyLength>599)
    {
      UartCopyLength=0;
    }
    else
    {
      ProcessUartCMD(UART0_RxBuf,UartCopyLength);
      UartCopyLength = 0;
    }
    UART0_RxLength = 0;
    UartCopyLength=0;
    Delay_device_1u(6000);
    Uart_IO_Init();
    return ( events ^ UART_RX_EVT);
  }
  /*��-------------------------------------------------------------------------*/ 
  if ( events & UART_TX2_EVT )
  {
    hal_USART0Init();
    Delay_device_1u(6000);
    hal_UARTWrite(UART_SEND_DOWN,LEN_UART_SEND_DOWN);
   // Delay_device_1u(500);
    //osal_pwrmgr_device(PWRMGR_BATTERY); //����͹���ģʽ
    osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,500);
    return ( events ^ UART_TX2_EVT);
  }
  /*��-------------------------------------------------------------------------*/ 
  if(events & OVERTIME_EVT)
  {
    if(CheckCode == 0)
    {
      if(ResponseCount<2)
      {
        ResponseCount++; 
        // if(UartCopyLength != 0) 
        // { 
        //osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,OVER_TIME);//���п������ݵķ��� 
        hal_UARTWrite(UART_SEND_DOWN,LEN_UART_SEND_DOWN);
        // } 
        osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,1500);
      }
      else
      {
        osal_stop_timerEx(UserApp_TaskID,OVERTIME_EVT);
        ResponseCount = 0; 
        wireless_commnd_flag =0;
      }
    }
    osal_pwrmgr_device(PWRMGR_BATTERY);
    Delay_device_1u(6000);
    Uart_IO_Init();
    return ( events ^ OVERTIME_EVT);
  } 
  /*��-------------------------------------------------------------------------*/
    if(events & WAKE_LOW_LEL_EVT)
    {
      OUT3_SBIT  = 0; 
      osal_start_timerEx(UserApp_TaskID,WAKE_UP_OUT_EVT,200);
      return (events ^ WAKE_LOW_LEL_EVT);
    }
  /*��-----------------------------------------------------------------------*/
  if(events & WAKE_UP_OUT_EVT)
  {
    OUT3_SBIT  = 1; 
    osal_start_timerEx( UserApp_TaskID,UART_TX2_EVT,5);  
    return ( events ^ WAKE_UP_OUT_EVT); 
 }
  /*��-------------------------------------------------------------------------*/
  if(events & FenBao_Report_EVT)//���зְ��ϴ����ݵĴ�����
  {
    FenBao_Reportup();
    return (events ^ FenBao_Report_EVT);  
  }
  /*��-------------------------------------------------------------------------*/ 
  if(events & Start_Joinnet_EVT)
  {  
    SceneSwitch_Mes_Write[0]=0x03;
    osal_nv_item_init(ZCD_NV_MES_FLASH ,1, NULL);
    osal_nv_write(ZCD_NV_MES_FLASH,0,1,&SceneSwitch_Mes_Write);//����Ϣд��NV
    WZCL_FindAndJoinNetwork();
    
    return ( events ^ Start_Joinnet_EVT); 
  }
  /*��-------------------------------------------------------------------------*/  
  if(events & Check_NetState_EVT)
  {
    if((devState != DEV_END_DEVICE)) //��������������
    {
      UART_SEND_DOWN[0] = 0xA1; 
      UART_SEND_DOWN[1] = 0x95; 
      UART_SEND_DOWN[2] = 0x00;
      UART_SEND_DOWN[3] = 0x01;
      UART_SEND_DOWN[4] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
      
      LEN_UART_SEND_DOWN = 5;
      osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 );
      //WakeLowLevel();
    }
    return (events ^ Check_NetState_EVT);
  }
  /*��-------------------------------------------------------------------------*/
  if(events & Leave_Network_EVT)
  {
    SceneSwitch_Count_Write[0]=0x00;
    osal_nv_item_init(ZCD_NV_SAVE_PARAMETER ,1, NULL);
    osal_nv_write(ZCD_NV_SAVE_PARAMETER ,0,1,&SceneSwitch_Count_Write);//����Ϣд��NV
    
    WZCL_LeaveAndRestoreFactory(); 
    return (events ^ Leave_Network_EVT);  
  }
  /*��-------------------------------------------------------------------------*/
  if(events & NET_Message_EVT)
  {
    if(Leave_NET_flag == 1)
    {
      UART_SEND_DOWN[1] = 0x94; 
    }
    else
    {
      UART_SEND_DOWN[1] = 0x93;  
    }
    UART_SEND_DOWN[0] = 0xA1;
    UART_SEND_DOWN[2] = 0x00; 
    UART_SEND_DOWN[3] = 0x01;
    UART_SEND_DOWN[4] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
    LEN_UART_SEND_DOWN = 5;
    osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10);
    //WakeLowLevel();
    
    Leave_NET_flag =0;
    return ( events ^ NET_Message_EVT);
  }
  /*��-------------------------------------------------------------------------*/
  if(events & Auto_Lock_EVT)
  {
    SampleApp_SendDeviceState(0x0102);  
    return ( events ^ Auto_Lock_EVT); 
  }
  /*��-------------------------------------------------------------------------*/
  if(events & SET_POLLTIME_EVT)
  {
   if(devState == DEV_END_DEVICE)
   {
      NLME_SetPollRate(4000);
    }
    return(events ^ SET_POLLTIME_EVT); 
  }
  return 0;
}
/*****************************************************************************/
void WakeLowLevel()
{
  OUT3_SBIT  = 0; 
  Delay_device_1u(800);
  OUT3_SBIT  = 1; 
  Delay_device_1u(2000);
  hal_UARTWrite(UART_SEND_DOWN,LEN_UART_SEND_DOWN);
  Delay_device_1u(500);
  //osal_pwrmgr_device(PWRMGR_BATTERY); //����͹���ģʽ
}
/*******************************************************************************
* @fn      UserAppHandleKeys
*
* @brief
*
* @param   shift  - .
* @param   keys   - .
*
* @return  none
*/
static void UserAppHandleKeys( uint8 shift, uint16 keys )
{
  if(keys & HAL_KEY_SW_1)
  {
    UserAppProcessKeyNwkAction();
  }
  else if(keys & HAL_KEY_SW_3)
  {
    if(shift)
    {
      /*����bindingʹ��*/
      //HalLedBlink ( HAL_LED_1, 2, 50, 1000 );
    }
    else
    {
      /*������������ʹ��*/
      //HalLedBlink ( HAL_LED_2, 2, 50, 1000 );
    }
  }
  else if(keys & HAL_KEY_SW_4)
  {
    if(shift)
    {
      /*����bindingʹ��*/
      //HalLedBlink ( HAL_LED_1, 3, 50, 1000 );
    }
    else
    {
      /*������������ʹ��*/
      //HalLedBlink ( HAL_LED_2, 3, 50, 1000 );
    }
  }
  else if(keys & HAL_KEY_SW_5)
  {
    if(shift)
    {
      /*����bindingʹ��*/
      //HalLedBlink ( HAL_LED_1, 4, 50, 1000 );
    }
    else
    {
      /*������������ʹ��*/
      //HalLedBlink ( HAL_LED_2, 4, 50, 1000 );
    }
  }
  else if(keys & HAL_KEY_SW_5)
  {
    
  }
  else if(keys & HAL_KEY_SW_6)
  {
    
  }
  else if(keys & HAL_KEY_SW_7)
  { 
    
  }
  else if(keys & HAL_KEY_SW_8)
  {  
    
  }
}
/*********************************************************************
* @fn      UserAppProcessKeyNwkAction
*
* @brief
*
*
* @param   none
*
* @return  none
*/
void UserAppProcessKeyNwkAction(void)
{
  if(KeyMultiClicks == 1)
  {
    if((devState == DEV_END_DEVICE) || (devState == DEV_ROUTER))
    {
      HalLedBlink ( HAL_LED_1, 1, 50, 1000 );
#if defined(LARGER_NETWORK)
      if(devState == DEV_END_DEVICE)
        WZCL_ActiveDevice();
#endif
    }
#ifndef HOLD_AUTO_START
    else
    {
      WZCL_FindAndJoinNetwork();
    }
#endif
  }
  else if(KeyMultiClicks == 2)
  {
    if( (devState==DEV_END_DEVICE) || (devState==DEV_ROUTER) )
    {
      WZCL_InterRssiReqMessage();
      BlinkLedForRssi = TRUE;
    }
  }
  else if(KeyMultiClicks == 4)
  {
    WZCL_FindAndJoinNetwork();
  }
#if defined(RTR_NWK)
  else if(KeyMultiClicks == 6)
  {
    if( (devState==DEV_END_DEVICE) || (devState==DEV_ROUTER) )
      WZCL_PermitJoin(240);
  }
  else if(KeyMultiClicks == 8)
  {
    if( (devState==DEV_END_DEVICE) || (devState==DEV_ROUTER) )
      WZCL_PermitJoin(0);
  }
  else if(KeyMultiClicks ==10)
  {
    HalLedSet ( HAL_LED_1 | HAL_LED_2, HAL_LED_MODE_ON);
    while(1);
  }
#endif
  else if(KeyHoldPress ==1)
  {
    if ( !(zdappHoldKeys & SW_BYPASS_START ))
    {
      WZCL_LeaveAndRestoreFactory();
    }
  }
}
/*******************************************************************************
* @fn      WZCL_CommandLeaveAndRestoreFactoryCallBack
*
* @brief   �豸�����ص��������������������Ͱ�������������
*
* @param   None
*
* @return  none
*/
void WZCL_CommandLeaveAndRestoreFactoryCallBack(void)
{
  if(ZG_BUILD_JOINING_TYPE && ZG_DEVICE_JOINING_TYPE )
    HalLedBlink ( HAL_LED_1, 4, 50, 1000 );
#if defined (ZDO_COORDINATOR)
  else  if(ZG_BUILD_COORDINATOR_TYPE && ZG_DEVICE_COORDINATOR_TYPE )
    HalLedBlink (HAL_LED_1, 6, 50, 500 );
#endif
}
/*******************************************************************************
* @fn      UserAppProcessCommandCallBack
*
* @brief   �������紫�ص�Ӧ����������
*
* @param   uint8 inMsgLen ʵ��Ӧ�����ݵĳ���
* @param   uint8 *inMsg  ʵ��������Ϣ�ĵ�ַ
* @param   uint8 *outMsgLen ʵ�ʴ洢��Ӧ�����ݳ��ȵĵ�ַ
* @param   uint8 *outMsg ʵ�ʴ洢��Ӧ���������ݵĵ�ַ
*
* @return  none
*/
void UserAppProcessCommandCallBack(uint8 inMsgLen, uint8 *inMsg,
                                   uint8 *outMsgLen, uint8 *outMsg)
{
  osal_pwrmgr_device(PWRMGR_ALWAYS_ON);
  
  uint8 i=0;
  uint8 CRC=0;
  UART_SEND_DOWN[0] = 0xA1; 
  
  if(wireless_commnd_flag == 0)
  {
    switch(inMsg[0])
    {
    case '1':
      {
        switch(inMsg[1])
        {
        case '1':
          {
            wireless_commnd_flag = 0x11;    //���߿���flag
            
            UART_SEND_DOWN[1] = 0x18;
            if((inMsg[2]>'0')&&(inMsg[2]<='9'))
            {
              UART_SEND_DOWN[4] = inMsg[2]-'0'; //���볤��
            }
            else if(inMsg[2]>'9')
            {
              UART_SEND_DOWN[4] = inMsg[2]-0x37; //���볤�� 
            }
            
            for(i=0;i<UART_SEND_DOWN[4]+1;i++)          //��������
            {
              UART_SEND_DOWN[i+5] = inMsg[i+3]-'0'; 
            }
            UART_SEND_DOWN[2]  = 0;
            UART_SEND_DOWN[3]  = UART_SEND_DOWN[4]+2;
            for(i=0;i<UART_SEND_DOWN[4]+5;i++)      
            {
              CRC += UART_SEND_DOWN[i]; 
            }
            UART_SEND_DOWN[UART_SEND_DOWN[4]+5] = CRC;
            
            LEN_UART_SEND_DOWN =  UART_SEND_DOWN[4]+6;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME);//2s��Ӧ���д�������
            osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //���п������ݵķ���
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
         /*-------------------------------------------------------------------*/  
        case '2':
          {
            wireless_commnd_flag = 0x12;   //ʱ��ͬ��flag
            
            UART_SEND_DOWN[1] = 0x10;  
            for(i=0;i<6;i++)
            {
              UART_SEND_DOWN[4+i] = (inMsg[(2*i)+2]-'0')*10+(inMsg[(2*i)+3]-'0'); //��+��+��+ʱ+��+��
            }
            UART_SEND_DOWN[2] = 0x00;
            UART_SEND_DOWN[3] = 7;
            for(i=0;i<10;i++)
            {
              CRC +=UART_SEND_DOWN[i];
            }
            UART_SEND_DOWN[10] = CRC;
            LEN_UART_SEND_DOWN = 11;
            
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,1000);//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //���п������ݵķ���
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
        /*--------------------------------------------------------------------*/  
        case '5':
          {
            wireless_commnd_flag = 0x15;   //����Ա��֤flag
            
            UART_SEND_DOWN[1] = 0x11; 
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[4] = inMsg[2]-'0'; //�ͻ�����
            UART_SEND_DOWN[5] = (inMsg[3]-'0')*10+(inMsg[4]-'0'); //�ͻ����
            UART_SEND_DOWN[6] = inMsg[5]-'0'; //�������к�
            
            if((inMsg[6]>'0')&&(inMsg[6]<='9'))
            {
              UART_SEND_DOWN[7] = inMsg[6]-'0'; //���볤��
            }
            else if(inMsg[6]>'9')
            {
              UART_SEND_DOWN[7] = inMsg[6]-0x37; //���볤�� 
            }
            
            for(i=0;i<UART_SEND_DOWN[7];i++)
            {
              UART_SEND_DOWN[i+8] = inMsg[i+7]-'0'; 
            }
            
            UART_SEND_DOWN[3] = UART_SEND_DOWN[7]+5;
            
            for(i=0;i<UART_SEND_DOWN[7]+8;i++)                //�������ݴ��������к�
            {
              CRC += UART_SEND_DOWN[i]; 
            }
            UART_SEND_DOWN[UART_SEND_DOWN[7]+8] = CRC;
            
            LEN_UART_SEND_DOWN =  UART_SEND_DOWN[3]+4;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //���п������ݵķ���
            //WakeLowLevel(); 
            *outMsgLen=0;
          }break;
         /*-------------------------------------------------------------------*/  
        case '6':
          {
            wireless_commnd_flag = 0x16;   //�·���̬������λ
            
            UART_SEND_DOWN[1] = 0x13;
            UART_SEND_DOWN[4] = 0x03;//�ͻ����ԣ���̬���룩
            UART_SEND_DOWN[5] = 0x01;//�������к�
            if((inMsg[2]>'0')&&(inMsg[2]<='9'))
            {
              UART_SEND_DOWN[6] = inMsg[2]-'0'; //���볤��
            }
            else if(inMsg[2]>'9')
            {
              UART_SEND_DOWN[6] = inMsg[2]-0x37; //���볤�� 
            }
            
            for(i=0;i<UART_SEND_DOWN[6];i++)         //��������
            {
              UART_SEND_DOWN[i+7] = inMsg[i+3]-'0'; 
            }
            
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = UART_SEND_DOWN[6]+4;//LEN
            
            for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //����У��ֵ
            {
              CRC += UART_SEND_DOWN[i];
            }
            UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
            LEN_UART_SEND_DOWN = UART_SEND_DOWN[3]+4; 
            
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
           // WakeLowLevel(); 
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //���п������ݵķ���
            *outMsgLen=0;
          }break;
          
        default:
          break;
        }
      }break;
    /**************************************************************************/  
    case '2':
      {
        switch(inMsg[1]) 
        {
        case '1':
          {
            wireless_commnd_flag = 0x21;   //���������flag
            
            UART_SEND_DOWN[1] = 0x13;
            UART_SEND_DOWN[4] = inMsg[2]-'0';//�ͻ�����
            UART_SEND_DOWN[5] = inMsg[3]-'0';//�������к�
            if((inMsg[4]>'0')&&(inMsg[4]<='9'))
            {
              UART_SEND_DOWN[6] = inMsg[4]-'0'; //���볤��
            }
            else if(inMsg[4]>'9')
            {
              UART_SEND_DOWN[6] = inMsg[4]-0x37; //���볤�� 
            }
            
            for(i=0;i<UART_SEND_DOWN[6];i++)         //��������
            {
              UART_SEND_DOWN[i+7] = inMsg[i+5]-'0'; 
            }
            
            if(UART_SEND_DOWN[4]==0x02) //��ʱ�û�
            {
              for(i=0;i<12;i++) //�����Чʱ��-ʧЧʱ��
              {
                UART_SEND_DOWN[UART_SEND_DOWN[6]+7+i] = (inMsg[UART_SEND_DOWN[6]+5+i*2]-'0')*10+(inMsg[UART_SEND_DOWN[6]+6+i*2]-'0');
              }
              
              UART_SEND_DOWN[2] = 0;
              UART_SEND_DOWN[3] = UART_SEND_DOWN[6]+4+12;//LEN
              
              for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //����У��ֵ
              {
                CRC += UART_SEND_DOWN[i];
              }
              UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
              LEN_UART_SEND_DOWN =  UART_SEND_DOWN[3]+4;
            }
            else     //����ʱ�û�
            {
              UART_SEND_DOWN[2] = 0;
              UART_SEND_DOWN[3] = UART_SEND_DOWN[6]+4;//LEN
              
              for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //����У��ֵ
              {
                CRC += UART_SEND_DOWN[i];
              }
              UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
              LEN_UART_SEND_DOWN = UART_SEND_DOWN[3]+4; 
            }
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���п������ݵķ��� 
            //WakeLowLevel();
            *outMsgLen=0;
          }
          break;
          /*------------------------------------------------------------------*/  
        case '2':
          {
            wireless_commnd_flag = 0x22;   //����ɾ��flag
            
            UART_SEND_DOWN[1] = 0x14; 
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x03;
            UART_SEND_DOWN[4] = inMsg[2]-'0';
            UART_SEND_DOWN[5] = (inMsg[3]-'0')*10+(inMsg[4]-'0');
            UART_SEND_DOWN[6] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]
              +UART_SEND_DOWN[3]+UART_SEND_DOWN[4]+UART_SEND_DOWN[5];
            
            LEN_UART_SEND_DOWN = 0x07;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���п������ݵķ���
           // WakeLowLevel();
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '3':
          {
            wireless_commnd_flag = 0x23;   //�����޸�flag
            
            UART_SEND_DOWN[1] = 0x15;
            UART_SEND_DOWN[4] = inMsg[2]-'0'; //�ͻ�����
            UART_SEND_DOWN[5] = (inMsg[3]-'0')*10+(inMsg[4]-'0');//�ͻ����
            UART_SEND_DOWN[6] = inMsg[5]-'0';//�������к�
            
            if((inMsg[6]>'0')&&(inMsg[6]<='9'))
            {
              UART_SEND_DOWN[7] = inMsg[6]-'0'; //���볤��
            }
            else if(inMsg[6]>'9')
            {
              UART_SEND_DOWN[7] = inMsg[6]-0x37; //���볤�� 
            }
            
            UART_SEND_DOWN[2] = 0;//LEN
            UART_SEND_DOWN[3] = UART_SEND_DOWN[7]+5;//LEN
            for(i=0;i<UART_SEND_DOWN[7];i++)
            {
              UART_SEND_DOWN[i+8] = inMsg[i+7]-'0'; 
            }
            
            for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //����У��ֵ
            {
              CRC += UART_SEND_DOWN[i];
            }
            UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
            LEN_UART_SEND_DOWN =  UART_SEND_DOWN[3]+4;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //���п������ݵķ��� 
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '4':
          {
            wireless_commnd_flag = 0x24;   //һ�����flag
            
            UART_SEND_DOWN[1]  = 0x16;
            UART_SEND_DOWN[2]  = 0;
            UART_SEND_DOWN[3]  = 0x01;
            UART_SEND_DOWN[4]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
            
            LEN_UART_SEND_DOWN = 5;
            // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���ݷ���
            *outMsgLen=0;
          }break;
          
        default:
          break;
        }
      }break;
      /******************************��ѯ����**********************************/
    case '3':
      {
        switch(inMsg[1])
        {
        case '1':
          {
            wireless_commnd_flag = 0x31;   //��ѯ����Աflag
            
            UART_SEND_DOWN[1] = 0x12;
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x02;
            UART_SEND_DOWN[4] = 0x00;
            UART_SEND_DOWN[5] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
            
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            //WakeLowLevel(); 
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //���п������ݵķ���
            
            if(devState == DEV_END_DEVICE)
           {
              NLME_SetPollRate(200);              //����poll��ʱ��
              osal_start_timerEx(UserApp_TaskID,SET_POLLTIME_EVT,8000); 
            }
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '2':
          {
            wireless_commnd_flag = 0x32;   //��ѯ��ͨ�û�flag
            
            UART_SEND_DOWN[1] = 0x12;
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x02;
            UART_SEND_DOWN[4] = 0x01;
            UART_SEND_DOWN[5] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���ݷ��� 
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '3':
          {
            wireless_commnd_flag = 0x33;   //��ѯ��ʱ�û�flag
            
            UART_SEND_DOWN[1] = 0x12;
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x02;
            UART_SEND_DOWN[4] = 0x02;
            UART_SEND_DOWN[5] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���ݷ��� 
            //WakeLowLevel(); 
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '4':
          {
            wireless_commnd_flag = 0x34;   //��ѯ����״̬flag
            
            UART_SEND_DOWN[1]  = 0x17;
            UART_SEND_DOWN[2]  = 0x00;
            UART_SEND_DOWN[3]  = 0x01;
            UART_SEND_DOWN[4]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1] +UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
            
            LEN_UART_SEND_DOWN = 5;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���ݷ��� 
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
           /*------------------------------------------------------------------*/  
        case '5':
          {
            wireless_commnd_flag = 0x35;   //��ѯ����ָ��ID��
            
            UART_SEND_DOWN[1]  = 0x1B;
            UART_SEND_DOWN[2]  = 0x00;
            UART_SEND_DOWN[3]  = 0x02;
            UART_SEND_DOWN[4]  = inMsg[2]-'0';
            UART_SEND_DOWN[5]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1] +UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���ݷ��� 
            //WakeLowLevel();
            *outMsgLen=0;
          }
          break;
          /*------------------------------------------------------------------*/  
         case '6':
          {
            wireless_commnd_flag = 0x36;   //��ѯ���߿�ƬID��
            
            UART_SEND_DOWN[1]  = 0x1C;
            UART_SEND_DOWN[2]  = 0x00;
            UART_SEND_DOWN[3]  = 0x02;
            UART_SEND_DOWN[4]  = inMsg[2]-'0';
            UART_SEND_DOWN[5]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1] +UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s��Ӧ���д�������
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //���ݷ��� 
            //WakeLowLevel();
            *outMsgLen=0;
          }
          break;
          /***********************************************************************/
        default:
          break;
        }
      }break;
      
    default:
      break;
    }
  }
  else
  {
    wireless_commnd_flag =0;
    osal_memset(inMsg,0,inMsgLen);
    *outMsgLen=0;
  }
}
/*******************************************************************************
* @fn      UserAppProcessReportingCommand
*
* @brief   ���ڴ����������ϱ���������/C/UPX  /C/USX  /C/UFX ��
*
* @param   uint8 cmd  ���ڿ����������ϱ�������
* @param   uint8 *inMsg  ���ڿ����������ϱ�������
*
* @return  none
*******************************************************************************/
void UserAppProcessReportingCommand(uint8 cmd ,uint8*inMsg)
{
  switch(cmd)
  {
  case 'P':
    
    break;
    
  case 'S':
    
    break;
    
  case 'F':
    
    break;
    /* ...... */
  default:
    break;
  }
}
/*******************************************************************************
* @fn      UserAppProcessBindingMessage
*
* @brief   �󶨿��ƴ�����
*
* @param   none
*
* @return  none
*/
void UserAppProcessBindingMessage(afIncomingMSGPacket_t * pkt)
{
  switch(pkt->endPoint)
  {
  case USERAPP_ENDPOINT1:
    
    break;
  case USERAPP_ENDPOINT2:
    
    break;
  case USERAPP_ENDPOINT3:
    
    break;
    
  default:
    break;
  }
}
/*********************************************************************
* @fn      UserAppProcessZDOMsgs
*
* @brief   ָʾ���豸�˵��״̬
*          �󶨳ɹ�3s��˸3��
*          ��ʧ��3s��˸6��
* @param   zdoIncomingMsg_t *pkt
*
* @return  none
*/
static void UserAppProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
  case End_Device_Bind_rsp:
#if defined(BLINK_LEDS)
    if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
    {
      HalLedBlink ( HAL_LED_1, 3, 50, 1000 );
    }
    else
    {
      HalLedBlink ( HAL_LED_1, 6, 50, 500 );
    }
#endif
    break;
  }
}
/*******************************************************************************
* @fn      UserAppJoinedNetworkCallBack
*
* @brief   �����ɹ��������ݺ���
*
* @param   none
*
* @return  none
*/
void UserAppJoinedNetworkCallBack(void)
{
  osal_stop_timerEx(UserApp_TaskID,Check_NetState_EVT);
  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
  
  if(devStartMode == MODE_JOIN)  //�״�������
  {
    SceneSwitch_Count_Write[0]=0x01;
    osal_nv_item_init(ZCD_NV_SAVE_PARAMETER ,1, NULL);
    osal_nv_write(ZCD_NV_SAVE_PARAMETER ,0,1,&SceneSwitch_Count_Write);//����Ϣд��NV
    
    osal_start_timerEx(UserApp_TaskID,NET_Message_EVT,10);//2s��Ӧ���д�������  
  }
  else
  {
    if(Commnd_JoinNet_Flag == 1)
    {
      Commnd_JoinNet_Flag =0;
      osal_start_timerEx(UserApp_TaskID,NET_Message_EVT,10);//2s��Ӧ���д�������
    }
  }
  
#if defined(LARGER_NETWORK)
  osal_start_timerEx( UserApp_TaskID,USERAPP_COMMAND_SEND_RSSI_MSG_EVT,600000);
#endif
}
/*******************************************************************************
* @fn      UserAppPowerOnJoinNwkDelayCallBack
*
* @brief   �����ϵ������ʱ������ʱ��
*
* @param   None
*
* @return  uint16   unit (s)
*/
uint16 UserAppPowerOnJoinNwkDelayCallBack(void)
{
  uint16 joinNwkDelay = 1;
  
  return  joinNwkDelay;
}
/*******************************************************************************
* @fn      UserAppStartNwkJoiningCallBack
*
* @brief   ���������ص�����
*
* @param   uint8 state
*
* @return  none
*/
void UserAppStartNwkJoiningCallBack(void)
{
  osal_nv_item_init(ZCD_NV_MES_FLASH ,1, NULL);
  osal_nv_read(ZCD_NV_MES_FLASH,0,1,&SceneSwitch_Mes_Read);
  if(SceneSwitch_Mes_Read[0] == 0x03) //������������
  {
    Commnd_JoinNet_Flag=1;
    
    SceneSwitch_Mes_Write[0]=0x00;
    osal_nv_item_init(ZCD_NV_MES_FLASH ,1, NULL);
    osal_nv_write(ZCD_NV_MES_FLASH,0,1,&SceneSwitch_Mes_Write);//����Ϣд��NV
    
    osal_nv_item_init(ZCD_NV_SAVE_PARAMETER ,1, NULL);
    osal_nv_read(ZCD_NV_SAVE_PARAMETER ,0,1,&SceneSwitch_Count_Read);
    if(SceneSwitch_Count_Read[0]!=1)
    {
      osal_start_timerEx(UserApp_TaskID,Check_NetState_EVT,60000);//һ���Ӻ��ѯ����״̬ 
    }  
  }
  SceneSwitch_Mes_Read[0] = 0;
}
/*******************************************************************************
* @fn      UserAppSendHeartBeatCallBack
*
* @brief   ���������ص�����,
*
* @param   void
*
* @return  none
*/
void UserAppSendHeartBeatCallBack(void)
{
  /* only used for test */
}
/*******************************************************************************
* @fn      UserAppCoordinatorOnlineCallback
*
* @brief   ֪ͨӦ�ò�Coordinator�豸�Ƿ��������У�ʵ�ʼ������л�����ʱ����ʱʱ��
*          ȡ���ڲ���devInfo->MinutesHeartWithApsAck���ýӿ�·���豸��Ч��
*          EndDevice��Ч;
*
* @param   offline   ���offline = TRUE;��ʾCoordinator�豸��������
*          ���offline = FALSE;��ʾCoordinator�豸������
* @return  none
*/
void UserAppCoordinatorOfflineCallback(bool offline)
{
  
}
/*******************************************************************************
* @fn      UserAppJoinNetworkBlinkCallback
*
* @brief   �������ƻص�����
*
* @param   uint8 state
*
* @return  none
*/
void UserAppJoinNetworkBlinkCallback(void)
{
  HalLedBlink ( HAL_LED_1, 1, 30, 500 );
}
/*******************************************************************************
* @fn      UserAppRssiBlinkLed
*
* @brief   �����ź�ֵǿ����˸LED��/��3s����ָʾ����˸�Ĵ��������ź�ֵ�Ĵ�С
*
* @param   none
*
* @return  none
*/
void UserAppRssiBlinkLed(wzclDevRssiRsp_t *pRsp)
{
  if(BlinkLedForRssi)
  {
    if ( !(zdappHoldKeys & SW_BYPASS_START) )
      HalLedBlink ( HAL_LED_1, pRsp->sigDowm.rssi/10, 50, 3000/(pRsp->sigDowm.rssi/10));
    BlinkLedForRssi = FALSE;
  }
}
/*******************************************************************************
* @fn      UserAppExtendLightBlink
*
* @brief   ���ڵ���ƾ�/��Ƕʽ���ص�L��������/�������
*          �����Ҫ���øù��ܣ��ں���UserAppSetDevInfo()������
*          devInfo->defaultBlinkEnable = FALSE;
*
* @param   none
*
* @return  none
*/
void UserAppExtendLightBlink(uint8 numblinks)
{
  
}
/*******************************************************************************
* @fn      UserAppPreSendRsMessageCallBack
*
* @brief   �û���������  iMsg�����ݣ���iMsg��Ϣ�ĳ��� *Len
*
* @param   uint8*iMsg
*          uint8* len
* @return  none
*/
void UserAppPreSendRsMessageCallBack( uint8 * rsMsg )
{
  rsMsg[0] = 0x4F;
  rsMsg[1] = 0x50;
}
/*******************************************************************************
* @fn      UserAppSetDeviceInfo
*
* @brief   �����豸��ͨ����Ϣ
*
* @param   wzclDeviceInfo_t *deviceinfo
*
* @return  none
*/
void UserAppSetDevInfo(wzclDevInfo_t *devInfo)
{
  devInfo->ctrlChar = 'E';
  devInfo->cmdIndCh = '/';
  devInfo->devId = 220;
  devInfo->rsLen = 2;
  /* if you want create a binding link ,set the parameter*/
  devInfo->extEpNums = 0;
  devInfo->lowBatteryAlarm.EnableAlarm = 0;
  devInfo->lowBatteryAlarm.DetectMode = MODE_HOUR;
  
  devInfo->MinutesHeartWithApsAck=30;
#if !defined(RTR_NWK)
  devInfo->maxPollFailureRetries = 7;
#endif
  devInfo->MinutesSendHeartToCoord = 30;
}
/*******************************************************************************
* @fn      UserAppAutoStartNwk
*
* @brief   Set device auto start joining network.if return TRUE ,auto joining
*          network after power on. otherwise, don't start joining network.
*
* @param
*
* @return  TRUE or FALSE,default return FALSE
*/
uint8 UserAppAutoStartNwk(void)
{
  return FALSE;
}
/*******************************************************************************
* @fn      UserAppSetPollRate
*
* @brief   ���� EndDevice�� PollRate
*
* @param   wzclPollControl_t *pollrate
*
* @return  none
*/
void UserAppSetPollRate(wzclPollControl_t *pollrate)
{
  if ( zdappHoldKeys & SW_BYPASS_START )
  {
    pollrate->shortPollInterval = 200;
  }
  else
  {
    pollrate->shortPollInterval = 4000;
//    pollrate->enablelongPoll = 1;
//    pollrate->longPollInterval  = 4000;
  }
}
/*******************************************************************************
* @fn      ChannelSelect
*
* @brief   �����ŵ�ѡ��
*
* @param   uint8 state
*
* @return  none
*/
uint32 ChannelSelect(void)
{
  uint32 ChannelList;
#if !defined(ewarm)
  if(CHANNEL_SELECT_BIT1)
  {
    if(CHANNEL_SELECT_BIT0)
      ChannelList =DEFAULT_CHANLIST_25;
    else
      ChannelList =DEFAULT_CHANLIST_21;
  }
  else
  {
    if(CHANNEL_SELECT_BIT0)
      ChannelList =DEFAULT_CHANLIST_17;
    else
      ChannelList =DEFAULT_CHANLIST_13;
  }
#else
  ChannelList =DEFAULT_CHANLIST_13;
#endif
  return ChannelList;
}
/*******************************************************************************
* @fn      Delay_device_1u
*
* @brief   ��ʱ����
*
* @param   uint32 microSecs
*
* @return  none
*/
void Delay_device_1u(uint32 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}
void SampleApp_SendDeviceState(uint16 sta)
{
  uint8 str[2];
  str[0]=sta>>8;//�����豸���Ͷ���ͬ��S��ʾ���ء�H��ʾ���� Ŀǰ�ֶ�����
  str[1]=sta;
  WZCL_SendDsMessage((uint8*)&str[0],2);//zhouj 061201
}
/*****************************************************************************/
void SampleApp_Message(uint8 Value) //zhouj 061201
{
  uint8 str[2];
  switch(Value)
  {
  case 0x20:     //������֤ʧ�ܣ�ϵͳ����
    str[0] = 0x02;
    str[1] = 0x1F;
    break;
    
  case 0x23:       //��������״̬�ϱ�
    str[0] = 0x02;
    str[1] = 0x1D;
    break;
    
  case 0x25:      //�û���������ϱ���ʾ
    str[0] = 0x02;
    str[1] = 0x1A;
    break;
    
  case 0x3E:      //�������������Ϣ�ϱ�
    str[0] = 0x02;
    str[1] = 0x1E;
    break;
    
  case 0x30:      //���Ƿѹ״̬�ϱ�
    str[0] = 0x02;
    str[1] = 0x1C;
    break;
    
  case 0x31:       //�����ϱ�
    str[0] = 0x02;
    str[1] = 0x0A;
    break;
    
  case 0x32:       //�������
    str[0] = 0x02;
    str[1] = 0x0B;
    break;
    
  case 0x36:        //���ظ�ʽ����Ϣ�ϱ�
    str[0] = 0x02;
    str[1] = 0x0C;
    break;
    
  default:
    break; 
  }
  WZCL_SendDsMessage((uint8*)&str[0],2);
}
/*****************************************************************************/
void SampleApp_Send_Message(uint8 Value) //zhouj 061201
{
  uint8 i=0;
  uint8 str[30];
  str[0] = 0x08; 
  
  switch(Value)
  {
  case 0x33:         //���������
    str[1] = 0x01; 
    break;
    
  case 0x34:          //����ɾ���û���Ϣ
    str[1] = 0x02; 
    break;
    
  case 0x35:         //�޸Ŀͻ���Ϣ���������ʱ�������
    str[1] = 0x03; 
    break;
    
  case 0x37:         //�Զ�ɾ����ʱ����
    str[1] = 0x04; 
    break;
    
  case 0x39:        //��ͨ�û���Ϣ�ϴ�
    str[1] = 0x06; 
    break;
    
  case 0x3A:       //��ʱ�û���Ϣ�ϴ�
    str[1] = 0x07; 
    break;
    
  case 0x3D:      //���ؿ�����Ϣ�ϱ������롢ָ�ƣ�Կ�ף�
    str[1] = 0x08; 
    break;
    
  case 0x24:       //���롢ָ�ƽٳ��ϱ�
    str[1] = 0x09; 
    break;
    
  case 0xAA:       //APP������Ϣ�ϱ�
    str[1] = 0x0A; 
    break;
    
  case 0x80:      //���������״̬�ϱ�
    str[1] = 0x0B; 
    break;
    
  case 0x3B:      //���������״̬�ϱ�
    str[1] = 0x0C; 
    break;
    
  case 0x51:
    str[1] = 0x0E; //�ϱ�����������û�
    break;
    
  case 0x52:
    str[1] = 0x0F; //�ϱ�����ɾ���û���Ϣ
    break;
    
  case 0x53:
    str[1] = 0x0D; //�ϱ��޸Ŀͻ���Ϣ
    break;
    
  default:
    break;
  }
  
  for(i=0;i<dataLenReportUp;i++)
  {
    str[i+2]= dataReportUp[i]; 
  }
  
  WZCL_SendDsMessage((uint8*)&str[0],dataLenReportUp+2);
  osal_memset(dataReportUp,0,60);
}
/*******************************************************************************/
void SampleApp_Send_Message_Ex(uint8 Value) //zhouj 061201
{
    uint8 i=0;
    uint8 str[30];
    str[0] = 0x09; 
    switch(Value)
   {
   case 0x70: //�������ָ��ID���ϴ�
     str[1] = 0x01;
     break;
     
   case 0x71: //������ӿ�ƬID���ϴ�
     str[1] = 0x02;
     break;
     
    case 0x72: //�������ָ��ID���ϴ�
     str[1] = 0x03;
     break;
     
   case 0x73: //������ӿ�ƬID���ϴ�
     str[1] = 0x04;
     break;
     
   default:
     break;
   }
    for(i=0;i<dataLenReportUp;i++)
  {
    str[i+2]= dataReportUp[i]; 
  }
  
  WZCL_SendDsMessage((uint8*)&str[0],dataLenReportUp+2);
  osal_memset(dataReportUp,0,60);
}
/*******************************************************************************/
void SampleApp_SendInMessage(void)
{
  uint8 i=0;
  uint8 str[30];
  
  for(i=0;i<dataLenReportUp;i++)
  {
    str[i]= dataReportUp[i]; 
  }
  WZCL_SendDsMessage((uint8*)&str[0],dataLenReportUp);
}
/************************************�ְ��ϴ�����************************************************/
void FenBao_Reportup(void)
{
  uint16 i=0;
  dataReportUp[3] = Pkg_cnt; //�ܰ���
  FenBao_cnt++;
  dataReportUp[2] = FenBao_cnt;    //������˼���
  
  switch(dataReportUp_flag)
  {
  case 0x01:         //��ͨ�û���Ϣ�ϱ�
  case 0x03:
    {
      if((Pkg_cnt == 1)&&(Pkg_cnt_flag == 1)) //��������
      {
        dataReportUp[0]=CommonUserData.Head_H = 0x08;
        if(dataReportUp_flag == 0x01)
        {
          dataReportUp[1]=CommonUserData.Head_L = 0x06;  
        }
        else if(dataReportUp_flag == 0x03)
        {
          dataReportUp[1]=CommonUserData.Head_L = 0x05; 
        }
        dataReportUp[2]=CommonUserData.CurrentPacket = 0x01;
        dataReportUp[3]=CommonUserData.TotalPackct = 0x01;
        dataReportUp[4]=CommonUserData.TotalUserNum = UART0_RxBuf[4]; //�ͻ�����
        dataReportUp[5]=CommonUserData.PasswordLen =  UART0_RxBuf[5]; //���볤��
        dataReportUp[6]=CommonUserData.UserNum =  UART0_RxBuf[6]; //�û����
        dataReportUp[7]=CommonUserData.PswGroupNum = 0x01; //��������
        dataReportUp[8]=CommonUserData.PswSerialNum = 0x01;//����ID
        
        // Password = osal_mem_alloc(CommonUserData.PasswordLen);
        
        for(i=0;i<CommonUserData.PasswordLen;i++)
        {
          dataReportUp[9+i]= UART0_RxBuf[9+i];
        }
        
        WZCL_SendDsMessage((uint8 *)&dataReportUp,9+CommonUserData.PasswordLen);
        dataReportUp_flag = 0;
        
        FenBao_cnt=0;
        Pkg_cnt=0;
        Pkg_cnt_flag=0;
      }
      //���� || δ����
      else if(((Pkg_cnt_flag != 0x01)&&(FenBao_cnt <= Pkg_cnt))||((Pkg_cnt_flag  == 0x01)&&(FenBao_cnt < Pkg_cnt))) 
      {       
        dataReportUp[0]=CommonUserEx.Head_H = 0x08;
        if(dataReportUp_flag == 0x01)
        {
          dataReportUp[1]=CommonUserData.Head_L = 0x06;  
        }
        else if(dataReportUp_flag == 0x03)
        {
          dataReportUp[1]=CommonUserData.Head_L = 0x05; 
        }
        dataReportUp[2]=CommonUserEx.CurrentPacket = FenBao_cnt; //��ǰ����
        dataReportUp[3]=CommonUserEx.TotalPackct   = Pkg_cnt;      //�ܰ���
        dataReportUp[4]=CommonUserEx.TotalUserNum  = UART0_RxBuf[4];//�ͻ�����
        if(FenBao_cnt == 1)
        {
          dataReportUp[5]= CommonUserEx.PasswordLen  = UART0_RxBuf[5];//��һ�����볤��
          dataReportUp[6]= CommonUserEx.UserNum      = UART0_RxBuf[6]; //�û����
          dataReportUp[7]= CommonUserEx.PswGroupNum  = 0x01; //��������
          dataReportUp[8]= CommonUserEx.PswSerialNum = 0x01;//����ID
          
          for(i=0;i<CommonUserEx.PasswordLen;i++)
          {
            dataReportUp[9+i] = UART0_RxBuf[9+i];
          }
          
          CommonUserBuffEx = CommonUserEx.PasswordLen;
          CommonUserBuffEx += 5+4;
          
          dataReportUp[9+CommonUserEx.PasswordLen] = CommonUserEx.PasswordLenEx = UART0_RxBuf[CommonUserBuffEx];
          dataReportUp[10+CommonUserEx.PasswordLen] = CommonUserEx.UserNumEx = UART0_RxBuf[CommonUserBuffEx+1];
          dataReportUp[11+CommonUserEx.PasswordLen] = CommonUserEx.PswGroupNumEx  = 0x01;
          dataReportUp[12+CommonUserEx.PasswordLen] = CommonUserEx.PswSerialNumEx = 0x01;
          
          for(i=0;i<CommonUserEx.PasswordLenEx;i++)
          {
            dataReportUp[13+CommonUserEx.PasswordLen+i]= UART0_RxBuf[CommonUserBuffEx+4+i];           //PasswordEx[i] = UART0_RxBuf[CommonUserBuffEx+4+i];
          }
          WZCL_SendDsMessage((uint8 *)&dataReportUp,13+CommonUserEx.PasswordLenEx+CommonUserEx.PasswordLen);
          osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,350); 
          //osal_memset((uint8 *)&CommonUserEx,0,13);
        }
        else
        {
          CommonUserBuffEx += 4+CommonUserEx.PasswordLenEx;
          
          dataReportUp[5]=CommonUserEx.PasswordLen  = UART0_RxBuf[CommonUserBuffEx];//��һ�����볤��
          dataReportUp[6]=CommonUserEx.UserNum      = UART0_RxBuf[CommonUserBuffEx+1]; //�û����
          dataReportUp[7]=CommonUserEx.PswGroupNum  = 0x01; //��������
          dataReportUp[8]=CommonUserEx.PswSerialNum = 0x01;//����ID
          
          for(i=0;i<CommonUserEx.PasswordLen;i++)
          {
            dataReportUp[9+i]= UART0_RxBuf[CommonUserBuffEx+4+i];
          }
          
          CommonUserBuffEx += 4+CommonUserEx.PasswordLen;
          
          dataReportUp[9+CommonUserEx.PasswordLen] = CommonUserEx.PasswordLenEx = UART0_RxBuf[CommonUserBuffEx];
          dataReportUp[10+CommonUserEx.PasswordLen] = CommonUserEx.UserNumEx = UART0_RxBuf[CommonUserBuffEx+1];
          dataReportUp[11+CommonUserEx.PasswordLen]= CommonUserEx.PswGroupNumEx = 0x01;
          dataReportUp[12+CommonUserEx.PasswordLen]= CommonUserEx.PswSerialNumEx = 0x01;
          
          for(i=0;i<CommonUserEx.PasswordLenEx;i++)
          {
            dataReportUp[13+CommonUserEx.PasswordLen+i]=UART0_RxBuf[i+4+CommonUserBuffEx];
          }
          
          WZCL_SendDsMessage((uint8 *)&dataReportUp,13+CommonUserEx.PasswordLenEx+CommonUserEx.PasswordLen);
          osal_memset(dataReportUp,0,40);
          osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,350);
        }
        if(FenBao_cnt == Pkg_cnt)
        {
          osal_stop_timerEx(UserApp_TaskID,FenBao_Report_EVT);
          osal_memset(dataReportUp,0,60);
          dataReportUp_flag = 0;
          FenBao_cnt=0;
          Pkg_cnt=0;
          Pkg_cnt_flag=0; 
          CommonUserBuffEx=0;
        }
      }
      //δ���������һ��
      else if((Pkg_cnt_flag == 1)&&(FenBao_cnt == Pkg_cnt)&&(Pkg_cnt != 1))
      {
        CommonUserBuffEx += 4+CommonUserEx.PasswordLenEx;
        dataReportUp[0]=CommonUserData.Head_H = 0x08;
        if(dataReportUp_flag == 0x01)
        {
          dataReportUp[1]=CommonUserData.Head_L = 0x06;  
        }
        else if(dataReportUp_flag == 0x03)
        {
          dataReportUp[1]=CommonUserData.Head_L = 0x05; 
        }
        dataReportUp[2]=CommonUserData.CurrentPacket = FenBao_cnt; //��ǰ����
        dataReportUp[3]=CommonUserData.TotalPackct =  Pkg_cnt;      //�ܰ���;
        dataReportUp[4]=CommonUserData.TotalUserNum = UART0_RxBuf[4]; //�ͻ�����
        dataReportUp[5]=CommonUserData.PasswordLen = UART0_RxBuf[CommonUserBuffEx]; //���볤��
        dataReportUp[6]=CommonUserData.UserNum =   UART0_RxBuf[CommonUserBuffEx+1]; //�û����
        dataReportUp[7]=CommonUserData.PswGroupNum = 0x01; //��������
        dataReportUp[8]=CommonUserData.PswSerialNum = 0x01;//����ID
        
        for(i=0;i<CommonUserData.PasswordLen;i++)
        {
          dataReportUp[9+i] = UART0_RxBuf[CommonUserBuffEx+4+i];
        }
        
        WZCL_SendDsMessage((uint8 *)&dataReportUp,9+CommonUserData.PasswordLen);
        
        dataReportUp_flag = 0;
        FenBao_cnt=0;
        Pkg_cnt=0;
        Pkg_cnt_flag=0;
        CommonUserBuffEx=0;
        osal_memset(dataReportUp,0,60);
        osal_stop_timerEx(UserApp_TaskID,FenBao_Report_EVT);
      }
    }break;
    /*--------------------------------------------------------------------------*/    
  case 0x02:        //��ʱ�û������Ϣ
    {
      if(FenBao_cnt == 1)//��������
      {
        dataReportUp[0]=CasualUserData.Head_H = 0x08;
        dataReportUp[1]=CasualUserData.Head_L = 0x07;
        dataReportUp[2]=CasualUserData.CurrentPacket = 0x01;
        dataReportUp[3]=CasualUserData.TotalPackct = Pkg_cnt;
        dataReportUp[4]=CasualUserData.TotalUserNum= UART0_RxBuf[4]; //�ͻ�����
        dataReportUp[5]=CasualUserData.PasswordLen =  UART0_RxBuf[5]; //���볤��
        dataReportUp[6]=CasualUserData.UserNum =  UART0_RxBuf[6]; //�û����
        dataReportUp[7]=CasualUserData.PswGroupNum = 0x01; //��������
        dataReportUp[8]=CasualUserData.PswSerialNum = 0x01;//����ID
        
        for(i=0;i<CasualUserData.PasswordLen;i++)
        {
          dataReportUp[9+i]= UART0_RxBuf[9+i];
        }
        
        for(i=0;i<12;i++)
        {
          dataReportUp[9+CasualUserData.PasswordLen+i]= UART0_RxBuf[9+CasualUserData.PasswordLen+i];
        }
        
        WZCL_SendDsMessage((uint8 *)&dataReportUp,9+CasualUserData.PasswordLen+12);
        if(Pkg_cnt != 1)
        {
          osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,450);
          osal_memset(dataReportUp,0,60);
        }
        else
        {
          dataReportUp_flag = 0;
          FenBao_cnt=0;
          Pkg_cnt=0;
          Pkg_cnt_flag=0;
          osal_memset(dataReportUp,0,60);
        }
      }
      else   //��ֹһ��
      {
        //CasualUserBuffEx+= 12+5+4+CasualUserData.PasswordLen;
        //CasualUserBuffEx+= 12+5+4;
        if(FenBao_cnt == 2)
        {
          CasualUserBuffEx+= 12+5+4+CasualUserData.PasswordLen; 
        }
        else if(FenBao_cnt >2)
        {
          CasualUserBuffEx+= 12+4+CasualUserData.PasswordLen; 
        }
        
        dataReportUp[0]=CasualUserData.Head_H = 0x08;
        dataReportUp[1]=CasualUserData.Head_L = 0x07;
        dataReportUp[2]=CasualUserData.CurrentPacket = FenBao_cnt;
        dataReportUp[3]=CasualUserData.TotalPackct = Pkg_cnt;
        dataReportUp[4]=CasualUserData.TotalUserNum= UART0_RxBuf[4]; //�ͻ�����
        dataReportUp[5]=CasualUserData.PasswordLen =  UART0_RxBuf[CasualUserBuffEx]; //���볤��
        dataReportUp[6]=CasualUserData.UserNum =  UART0_RxBuf[CasualUserBuffEx+1]; //�û����
        dataReportUp[7]=CasualUserData.PswGroupNum = 0x01; //��������
        dataReportUp[8]=CasualUserData.PswSerialNum = 0x01;//����ID
        
        for(i=0;i<CasualUserData.PasswordLen;i++)
        {
          dataReportUp[9+i]= UART0_RxBuf[CasualUserBuffEx+4+i];
        }
        
        for(i=0;i<12;i++)
        {
          dataReportUp[9+CasualUserData.PasswordLen+i]=CasualUserData.PswTime[i] = UART0_RxBuf[CasualUserBuffEx+4+CasualUserData.PasswordLen+i];
        }
        
        WZCL_SendDsMessage((uint8 *)&dataReportUp,9+CasualUserData.PasswordLen+12); 
        osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,450);
        
        if(FenBao_cnt == Pkg_cnt)
        {
          osal_stop_timerEx( UserApp_TaskID,FenBao_Report_EVT);
          dataReportUp_flag = 0;
          FenBao_cnt=0;
          Pkg_cnt=0;
          Pkg_cnt_flag=0;
          CommonUserBuffEx=0;
          CasualUserBuffEx=0;
          osal_memset(dataReportUp,0,60);
        }
      }
    }
    break;
    /*------------------------------------------------------------------------*/    
  default:
    break;
  }
}
/******************************************************************************/
void Elock_Function_Test(void)
{
  uint8 UART_SEND_DOWN[8]={0};
  uint8 LEN_UART_SEND_DOWN=0;
  
  SampleApp_SendDeviceState(0x0F01);//�����źŲ���
  
  //�ж��ѽ��룬���д����Է����� 
  UART_SEND_DOWN[0] = 0xA1;
  UART_SEND_DOWN[1] = 0x00;
  UART_SEND_DOWN[2] = 0x01;
  UART_SEND_DOWN[3] = 0xA2;
  UART_SEND_DOWN[4] = 0xA1;
  UART_SEND_DOWN[5] = 0x00;
  UART_SEND_DOWN[6] = 0x01;
  UART_SEND_DOWN[7] = 0xA2;
  LEN_UART_SEND_DOWN = 8;
  UART0_RxLength = 0;
  hal_UARTWrite((uint8 *)&UART_SEND_DOWN,LEN_UART_SEND_DOWN);
  Delay_device_1u(100000);
  
  if(UART0_RxLength < 8)
  {
    SampleApp_SendDeviceState(0x0E01);//���ڲ���ʧ��
    UART0_RxLength = 0;
  }
  
  if((UART0_RxLength == 8) &&(UART0_RxBuf[0]==0xA1)&&(UART0_RxBuf[1]==0x00)&&
     (UART0_RxBuf[2]==0x01)&&(UART0_RxBuf[3]==0xA2))
  {
    SampleApp_SendDeviceState(0x0F02);//���ڲ��Գɹ�
    UART0_RxLength = 0; 
  }
  
  OUT3_SBIT = 1;
  Delay_device_1u(100000);
  OUT3_SBIT = 0;
  Delay_device_1u(100000);
  OUT3_SBIT = 1;
  Delay_device_1u(100000);
  OUT3_SBIT = 0;
  Delay_device_1u(100000);
  OUT3_SBIT = 1;
  SampleApp_SendDeviceState(0x0F03);//��������ɹ�
}
/***************** (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL***********/