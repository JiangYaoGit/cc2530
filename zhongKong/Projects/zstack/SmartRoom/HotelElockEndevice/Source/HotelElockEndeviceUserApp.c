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

uint8 UART_SEND_DOWN[50];    //模块收到上位机数据后，给设备发送的命令
uint8 LEN_UART_SEND_DOWN;   //发送数据的长度

uint8  dataReportUp[60];
uint16 dataLenReportUp;   

extern uint8  UART0_RxBuf[600];

uint16 UartCopyLength = 0;
uint16 Custer_num_total =0;//客户数量累计

uint8  wireless_commnd_flag = 0;//无线命令标记位 
uint8  Pkg_flag;//整数分包标志
uint8  Pkg_cnt,Pkg_cnt_Mod;
uint8  FenBao_cnt=0;
uint8  Pkg_cnt_flag=0;

uint8 dataReportUp_flag=0;
uint8 ResponseCount = 0; //应答次数
uint8 CheckCode     = 0; //密码校验
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
* @brief   错误回应
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
* @brief   正确回应
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
* @brief   处理串口数据
* @param   uint8 *datBuf ,uint8 length
* @return  none
*/
uint8 ProcessUartCMD( uint8 *datbuf ,uint16 length)
{
  uint16 i;
  uint8 Check=0;
  
  if(datbuf[0]==0xA1)     //包头正确
  {
    UART_SEND_DOWN[0]=0xA1;
    for(i=0;i<length-1;i++)
    {
      Check += datbuf[i];  //计算校验值 
    }
    //    if(Check != datbuf[length-1])  //数据校验错误
    //    {
    //      UART_CONFIRM_ERROR();
    //      return 0;
    //    }
    UART_SEND_DOWN[0] = 0xA1;
    osal_stop_timerEx (UserApp_TaskID,OVERTIME_EVT);
    switch(datbuf[1])
    {
      /****************************确认正确************************************/ 
    case 0x00:  
      {
        ResponseError =0; 
        wireless_commnd_flag=0; 
      }break;      
      /**************************校验或者返回错误******************************/    
    case 0x01:  
      {
        CheckCode = 1;
        ResponseError++; 
        if(ResponseError<3) //小于3次则要再次进行发送
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
          case 0x11:      //密码验证APP开门失
            SampleApp_SendDeviceState(0x0A01);     
            break;
            
          case 0x12:      //时间校准+动态密码失败
            SampleApp_SendDeviceState(0x0A02);     
            break;
            
          case 0x15:      //管理员验证失败
            SampleApp_SendDeviceState(0x0A00);     
            break;
            
          case 0x16:      //动态密码下发失败
            SampleApp_SendDeviceState(0x0A07);     
            break;
            
          case 0x21:     //添加新用户失败
            SampleApp_SendDeviceState(0x0A03);     
            break;
            
          case 0x22:     //名单删除失败
            SampleApp_SendDeviceState(0x0A04);     
            break;
            
          case 0x23:     //修改密码失败
            SampleApp_SendDeviceState(0x0A05);     
            break;
            
          case 0x24:    //一键清除失败
            SampleApp_SendDeviceState(0x0A06);     
            break;
            
          default:
            break;
          }
          return 0;
        }
      }break;
     /************密码校验错误（针对管理员密码验证+APP开锁密码验证）***********/       
    case 0x02:
      {
        UART_CONFIRM_TRUE();
        switch(wireless_commnd_flag)
        {          
        case 0x15:     //管理员密码错误 
          {
            SampleApp_SendDeviceState(0x0A11);
          }     
          break;
          
        default:
          break;
        }
      }break;
      /*************APP添加名单--/密码重复（重操作）/添加失败/添加已满*********/       
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
          SampleApp_SendDeviceState(0x0A20); //普通用户添加已满
          break;
          
        case 0x02:
          SampleApp_SendDeviceState(0x0A21); //临时用户添加已满
          break;
          
        default:
          break;
        }
      }break;
      /************************删除用户失败/修改密码失败/APP开锁失败********************************/
    case 0x0A://删除用户失败
      {
        UART_CONFIRM_TRUE(); 
        SampleApp_SendDeviceState(0x0A04); 
      }break;
      
    case 0x0B: //修改密码失败
      {
        UART_CONFIRM_TRUE(); 
        SampleApp_SendDeviceState(0x0A05);
      }break; 
      
    case 0x0D:  //APP开锁失败
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A01);
      }break;
      /************************网络授时********************************/
    case 0x04:  //网络授时成功
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0103);
      }break;
      
    case 0x08:  //网络授时失败
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A02); 
      }break;
      /************************管理员认证********************************/  
    case 0x05:  //认证成功
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0105); 
      }break;
      
    case 0x07:  //认证失败
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A00); 
      }break;
      /*************************一键清除********************************/
    case 0x06: //一键清除成功
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0224);   
      }break;
      
    case 0x0C: //一键清除失败
      {
        UART_CONFIRM_TRUE();
        SampleApp_SendDeviceState(0x0A06);   
      }break; 
      /**************************各种不带参数信息上报***********************/       
    case 0x20:       //连续认证失败，系统锁定
    case 0x23:       //门锁被撬状态上报
    case 0x25:        //用户添加已满
    case 0x30:       //电池欠压状态上报
    case 0x31:       //门内保险状态上报
    case 0x32:       //解除门内保险
    case 0x36:       //本地数据格式化上报
    case 0x3E:       //门锁解除锁定信息上报
      {
        if(datbuf[3] == 0x02)
        {
          UART_CONFIRM_TRUE();
          SampleApp_Message(datbuf[1]);//各种状态上报
        }
        else if(datbuf[3]!=0x01) 
        {
          UART_CONFIRM_ERROR();
        }
        else
        {
          UART_CONFIRM_TRUE();
          SampleApp_Message(datbuf[1]);//各种状态上报
        }
      }break;
      /*****************************入网指令*******************************/     
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
      /*****************************退网命令********************************/     
    case 0x41:   
      {
        if((datbuf[2]==0x00)&&(datbuf[3]==0x01))
        {
          UART_CONFIRM_TRUE();
          
          Leave_NET_flag=1;
          osal_start_timerEx(UserApp_TaskID,NET_Message_EVT,200);//2s后应该有串口数据
          osal_start_timerEx(UserApp_TaskID,Leave_Network_EVT,1500);//2s后应该有串口数据
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
      }break; 
      /************************各种带参数上报**********************************/
    case 0x3B:                           //查询锁具状态上报
    case 0x24:                          //密码或指纹防劫持
    case 0x33:                          //本地添加新用户信息
    case 0x34:                           //本地删除用户信息
    case 0x35:                           //本地修改客户信息
    case 0x37:                           //自动删除临时用户信息
    case 0x80:                           //锁具方舌状态上报
      {
        if(datbuf[3]!=0x00) //LEN不为0 
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
      /********************************禁止/允许远程操作**********************/
    case 0x60:
      {
        if(datbuf[3]!=0x00) //LEN不为0 
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
      /********************************虚掩**********************/
    case 0x61:
      {
        if(datbuf[3]!=0x00) //LEN不为0 
        {
          UART_CONFIRM_TRUE();
          SampleApp_SendDeviceState(0x0302); 
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
      }break;
      /*************************开锁信息上报***********************************/
    case 0x3D:
      {
        if(datbuf[3]!=0x00) //LEN不为0 
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
          osal_start_timerEx(UserApp_TaskID,Auto_Lock_EVT,5000);//2s后应该有串口数据
        }
        else
        {
          UART_CONFIRM_ERROR();
        }
        return 1;
      }break;
      /************************************************************************/
    case 0x50:                          //回传新加用户的ID
      {
        if(datbuf[3]!=0x00) //LEN不为0 
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
        dataReportUp[0] = 0x08; //表示分包数据上传
        dataReportUp[1] = 0x05; //表示分包数据上传
        
        dataReportUp_flag = 0x03;
        Pkg_cnt = datbuf[4]/2;  //分包的包数
        if(datbuf[4]%2 !=0)
        {
          Pkg_cnt = Pkg_cnt+1; 
          Pkg_cnt_flag = 1;      //未整除
        }
        osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,300); //每300ms 分包上传
      }break;
      /************************************************************************/
    case 0x39:                           //普通用户信息上传
    case 0x3A:                           //临时用户信息上传
      {
        Custer_num_total = datbuf[2]<<8|datbuf[3];
        if(Custer_num_total != 0) //LEN不为0
        {
          UART_CONFIRM_TRUE();
          switch(datbuf[1])
          {
          case 0x39:
            {
              dataReportUp[0] = 0x08; //表示分包数据上传
              dataReportUp[1] = 0x06; //表示分包数据上传
              
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
                Pkg_cnt = datbuf[4]/2;  //分包的包数
                if(datbuf[4]%2 !=0)
                {
                  Pkg_cnt = Pkg_cnt+1; 
                  Pkg_cnt_flag = 1;      //未整除
                }
                osal_start_timerEx( UserApp_TaskID,FenBao_Report_EVT,300); //每300ms 分包上传
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
              
              dataReportUp[0] = 0x08; //表示分包数据上传 
              dataReportUp[1] = 0x07; //表示分包数据上传
              
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
                osal_start_timerEx(UserApp_TaskID,FenBao_Report_EVT,300); //每300ms 分包上传
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
    case 0x70:       //本地添加指纹ID号上传
    case 0x71:       //本地添加卡片ID号上传
    case 0x72:       //被动上传指纹ID号上传
    case 0x73:       //被动上传卡片ID号上传
      {
        if(datbuf[3]!=0x00) //LEN不为0 
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
* @brief   将串口初始化为普通IO口，输入，3态
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
    }//end： while ( MSGpkt )
    
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
  /*—-------------------------------------------------------------------------*/  
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
  /*—-------------------------------------------------------------------------*/ 
  if ( events & UART_TX2_EVT )
  {
    hal_USART0Init();
    Delay_device_1u(6000);
    hal_UARTWrite(UART_SEND_DOWN,LEN_UART_SEND_DOWN);
   // Delay_device_1u(500);
    //osal_pwrmgr_device(PWRMGR_BATTERY); //进入低功耗模式
    osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,500);
    return ( events ^ UART_TX2_EVT);
  }
  /*—-------------------------------------------------------------------------*/ 
  if(events & OVERTIME_EVT)
  {
    if(CheckCode == 0)
    {
      if(ResponseCount<2)
      {
        ResponseCount++; 
        // if(UartCopyLength != 0) 
        // { 
        //osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,OVER_TIME);//进行开锁数据的发送 
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
  /*—-------------------------------------------------------------------------*/
    if(events & WAKE_LOW_LEL_EVT)
    {
      OUT3_SBIT  = 0; 
      osal_start_timerEx(UserApp_TaskID,WAKE_UP_OUT_EVT,200);
      return (events ^ WAKE_LOW_LEL_EVT);
    }
  /*—-----------------------------------------------------------------------*/
  if(events & WAKE_UP_OUT_EVT)
  {
    OUT3_SBIT  = 1; 
    osal_start_timerEx( UserApp_TaskID,UART_TX2_EVT,5);  
    return ( events ^ WAKE_UP_OUT_EVT); 
 }
  /*—-------------------------------------------------------------------------*/
  if(events & FenBao_Report_EVT)//进行分包上传数据的处理工作
  {
    FenBao_Reportup();
    return (events ^ FenBao_Report_EVT);  
  }
  /*—-------------------------------------------------------------------------*/ 
  if(events & Start_Joinnet_EVT)
  {  
    SceneSwitch_Mes_Write[0]=0x03;
    osal_nv_item_init(ZCD_NV_MES_FLASH ,1, NULL);
    osal_nv_write(ZCD_NV_MES_FLASH,0,1,&SceneSwitch_Mes_Write);//空信息写入NV
    WZCL_FindAndJoinNetwork();
    
    return ( events ^ Start_Joinnet_EVT); 
  }
  /*—-------------------------------------------------------------------------*/  
  if(events & Check_NetState_EVT)
  {
    if((devState != DEV_END_DEVICE)) //不在网络的情况下
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
  /*—-------------------------------------------------------------------------*/
  if(events & Leave_Network_EVT)
  {
    SceneSwitch_Count_Write[0]=0x00;
    osal_nv_item_init(ZCD_NV_SAVE_PARAMETER ,1, NULL);
    osal_nv_write(ZCD_NV_SAVE_PARAMETER ,0,1,&SceneSwitch_Count_Write);//空信息写入NV
    
    WZCL_LeaveAndRestoreFactory(); 
    return (events ^ Leave_Network_EVT);  
  }
  /*—-------------------------------------------------------------------------*/
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
  /*—-------------------------------------------------------------------------*/
  if(events & Auto_Lock_EVT)
  {
    SampleApp_SendDeviceState(0x0102);  
    return ( events ^ Auto_Lock_EVT); 
  }
  /*—-------------------------------------------------------------------------*/
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
  //osal_pwrmgr_device(PWRMGR_BATTERY); //进入低功耗模式
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
      /*按键binding使用*/
      //HalLedBlink ( HAL_LED_1, 2, 50, 1000 );
    }
    else
    {
      /*按键正常控制使用*/
      //HalLedBlink ( HAL_LED_2, 2, 50, 1000 );
    }
  }
  else if(keys & HAL_KEY_SW_4)
  {
    if(shift)
    {
      /*按键binding使用*/
      //HalLedBlink ( HAL_LED_1, 3, 50, 1000 );
    }
    else
    {
      /*按键正常控制使用*/
      //HalLedBlink ( HAL_LED_2, 3, 50, 1000 );
    }
  }
  else if(keys & HAL_KEY_SW_5)
  {
    if(shift)
    {
      /*按键binding使用*/
      //HalLedBlink ( HAL_LED_1, 4, 50, 1000 );
    }
    else
    {
      /*按键正常控制使用*/
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
* @brief   设备退网回调函数，包含命令退网和按键本地退网。
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
* @brief   处理网络传回的应用命令数据
*
* @param   uint8 inMsgLen 实际应用数据的长度
* @param   uint8 *inMsg  实际输入消息的地址
* @param   uint8 *outMsgLen 实际存储的应用数据长度的地址
* @param   uint8 *outMsg 实际存储的应用数据内容的地址
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
            wireless_commnd_flag = 0x11;    //无线开锁flag
            
            UART_SEND_DOWN[1] = 0x18;
            if((inMsg[2]>'0')&&(inMsg[2]<='9'))
            {
              UART_SEND_DOWN[4] = inMsg[2]-'0'; //密码长度
            }
            else if(inMsg[2]>'9')
            {
              UART_SEND_DOWN[4] = inMsg[2]-0x37; //密码长度 
            }
            
            for(i=0;i<UART_SEND_DOWN[4]+1;i++)          //密码数据
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
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME);//2s后应该有串口数据
            osal_start_timerEx(UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //进行开锁数据的发送
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
         /*-------------------------------------------------------------------*/  
        case '2':
          {
            wireless_commnd_flag = 0x12;   //时间同步flag
            
            UART_SEND_DOWN[1] = 0x10;  
            for(i=0;i<6;i++)
            {
              UART_SEND_DOWN[4+i] = (inMsg[(2*i)+2]-'0')*10+(inMsg[(2*i)+3]-'0'); //年+月+日+时+分+秒
            }
            UART_SEND_DOWN[2] = 0x00;
            UART_SEND_DOWN[3] = 7;
            for(i=0;i<10;i++)
            {
              CRC +=UART_SEND_DOWN[i];
            }
            UART_SEND_DOWN[10] = CRC;
            LEN_UART_SEND_DOWN = 11;
            
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,1000);//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //进行开锁数据的发送
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
        /*--------------------------------------------------------------------*/  
        case '5':
          {
            wireless_commnd_flag = 0x15;   //管理员认证flag
            
            UART_SEND_DOWN[1] = 0x11; 
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[4] = inMsg[2]-'0'; //客户属性
            UART_SEND_DOWN[5] = (inMsg[3]-'0')*10+(inMsg[4]-'0'); //客户编号
            UART_SEND_DOWN[6] = inMsg[5]-'0'; //密码序列号
            
            if((inMsg[6]>'0')&&(inMsg[6]<='9'))
            {
              UART_SEND_DOWN[7] = inMsg[6]-'0'; //密码长度
            }
            else if(inMsg[6]>'9')
            {
              UART_SEND_DOWN[7] = inMsg[6]-0x37; //密码长度 
            }
            
            for(i=0;i<UART_SEND_DOWN[7];i++)
            {
              UART_SEND_DOWN[i+8] = inMsg[i+7]-'0'; 
            }
            
            UART_SEND_DOWN[3] = UART_SEND_DOWN[7]+5;
            
            for(i=0;i<UART_SEND_DOWN[7]+8;i++)                //密码数据带密码序列号
            {
              CRC += UART_SEND_DOWN[i]; 
            }
            UART_SEND_DOWN[UART_SEND_DOWN[7]+8] = CRC;
            
            LEN_UART_SEND_DOWN =  UART_SEND_DOWN[3]+4;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //进行开锁数据的发送
            //WakeLowLevel(); 
            *outMsgLen=0;
          }break;
         /*-------------------------------------------------------------------*/  
        case '6':
          {
            wireless_commnd_flag = 0x16;   //下发动态密码标记位
            
            UART_SEND_DOWN[1] = 0x13;
            UART_SEND_DOWN[4] = 0x03;//客户属性（动态密码）
            UART_SEND_DOWN[5] = 0x01;//密码序列号
            if((inMsg[2]>'0')&&(inMsg[2]<='9'))
            {
              UART_SEND_DOWN[6] = inMsg[2]-'0'; //密码长度
            }
            else if(inMsg[2]>'9')
            {
              UART_SEND_DOWN[6] = inMsg[2]-0x37; //密码长度 
            }
            
            for(i=0;i<UART_SEND_DOWN[6];i++)         //密码数据
            {
              UART_SEND_DOWN[i+7] = inMsg[i+3]-'0'; 
            }
            
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = UART_SEND_DOWN[6]+4;//LEN
            
            for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //计算校验值
            {
              CRC += UART_SEND_DOWN[i];
            }
            UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
            LEN_UART_SEND_DOWN = UART_SEND_DOWN[3]+4; 
            
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
           // WakeLowLevel(); 
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //进行开锁数据的发送
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
            wireless_commnd_flag = 0x21;   //添加新名单flag
            
            UART_SEND_DOWN[1] = 0x13;
            UART_SEND_DOWN[4] = inMsg[2]-'0';//客户属性
            UART_SEND_DOWN[5] = inMsg[3]-'0';//密码序列号
            if((inMsg[4]>'0')&&(inMsg[4]<='9'))
            {
              UART_SEND_DOWN[6] = inMsg[4]-'0'; //密码长度
            }
            else if(inMsg[4]>'9')
            {
              UART_SEND_DOWN[6] = inMsg[4]-0x37; //密码长度 
            }
            
            for(i=0;i<UART_SEND_DOWN[6];i++)         //密码数据
            {
              UART_SEND_DOWN[i+7] = inMsg[i+5]-'0'; 
            }
            
            if(UART_SEND_DOWN[4]==0x02) //临时用户
            {
              for(i=0;i<12;i++) //算出生效时间-失效时间
              {
                UART_SEND_DOWN[UART_SEND_DOWN[6]+7+i] = (inMsg[UART_SEND_DOWN[6]+5+i*2]-'0')*10+(inMsg[UART_SEND_DOWN[6]+6+i*2]-'0');
              }
              
              UART_SEND_DOWN[2] = 0;
              UART_SEND_DOWN[3] = UART_SEND_DOWN[6]+4+12;//LEN
              
              for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //计算校验值
              {
                CRC += UART_SEND_DOWN[i];
              }
              UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
              LEN_UART_SEND_DOWN =  UART_SEND_DOWN[3]+4;
            }
            else     //非临时用户
            {
              UART_SEND_DOWN[2] = 0;
              UART_SEND_DOWN[3] = UART_SEND_DOWN[6]+4;//LEN
              
              for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //计算校验值
              {
                CRC += UART_SEND_DOWN[i];
              }
              UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
              LEN_UART_SEND_DOWN = UART_SEND_DOWN[3]+4; 
            }
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //进行开锁数据的发送 
            //WakeLowLevel();
            *outMsgLen=0;
          }
          break;
          /*------------------------------------------------------------------*/  
        case '2':
          {
            wireless_commnd_flag = 0x22;   //名单删除flag
            
            UART_SEND_DOWN[1] = 0x14; 
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x03;
            UART_SEND_DOWN[4] = inMsg[2]-'0';
            UART_SEND_DOWN[5] = (inMsg[3]-'0')*10+(inMsg[4]-'0');
            UART_SEND_DOWN[6] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]
              +UART_SEND_DOWN[3]+UART_SEND_DOWN[4]+UART_SEND_DOWN[5];
            
            LEN_UART_SEND_DOWN = 0x07;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //进行开锁数据的发送
           // WakeLowLevel();
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '3':
          {
            wireless_commnd_flag = 0x23;   //密码修改flag
            
            UART_SEND_DOWN[1] = 0x15;
            UART_SEND_DOWN[4] = inMsg[2]-'0'; //客户属性
            UART_SEND_DOWN[5] = (inMsg[3]-'0')*10+(inMsg[4]-'0');//客户编号
            UART_SEND_DOWN[6] = inMsg[5]-'0';//密码序列号
            
            if((inMsg[6]>'0')&&(inMsg[6]<='9'))
            {
              UART_SEND_DOWN[7] = inMsg[6]-'0'; //密码长度
            }
            else if(inMsg[6]>'9')
            {
              UART_SEND_DOWN[7] = inMsg[6]-0x37; //密码长度 
            }
            
            UART_SEND_DOWN[2] = 0;//LEN
            UART_SEND_DOWN[3] = UART_SEND_DOWN[7]+5;//LEN
            for(i=0;i<UART_SEND_DOWN[7];i++)
            {
              UART_SEND_DOWN[i+8] = inMsg[i+7]-'0'; 
            }
            
            for(i=0;i<UART_SEND_DOWN[3]+3;i++)   //计算校验值
            {
              CRC += UART_SEND_DOWN[i];
            }
            UART_SEND_DOWN[UART_SEND_DOWN[3]+3] = CRC;
            LEN_UART_SEND_DOWN =  UART_SEND_DOWN[3]+4;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //进行开锁数据的发送 
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '4':
          {
            wireless_commnd_flag = 0x24;   //一键清除flag
            
            UART_SEND_DOWN[1]  = 0x16;
            UART_SEND_DOWN[2]  = 0;
            UART_SEND_DOWN[3]  = 0x01;
            UART_SEND_DOWN[4]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
            
            LEN_UART_SEND_DOWN = 5;
            // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //数据发送
            *outMsgLen=0;
          }break;
          
        default:
          break;
        }
      }break;
      /******************************查询命令**********************************/
    case '3':
      {
        switch(inMsg[1])
        {
        case '1':
          {
            wireless_commnd_flag = 0x31;   //查询管理员flag
            
            UART_SEND_DOWN[1] = 0x12;
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x02;
            UART_SEND_DOWN[4] = 0x00;
            UART_SEND_DOWN[5] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
            
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            //WakeLowLevel(); 
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10); //进行开锁数据的发送
            
            if(devState == DEV_END_DEVICE)
           {
              NLME_SetPollRate(200);              //调整poll的时间
              osal_start_timerEx(UserApp_TaskID,SET_POLLTIME_EVT,8000); 
            }
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '2':
          {
            wireless_commnd_flag = 0x32;   //查询普通用户flag
            
            UART_SEND_DOWN[1] = 0x12;
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x02;
            UART_SEND_DOWN[4] = 0x01;
            UART_SEND_DOWN[5] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //数据发送 
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '3':
          {
            wireless_commnd_flag = 0x33;   //查询临时用户flag
            
            UART_SEND_DOWN[1] = 0x12;
            UART_SEND_DOWN[2] = 0;
            UART_SEND_DOWN[3] = 0x02;
            UART_SEND_DOWN[4] = 0x02;
            UART_SEND_DOWN[5] = UART_SEND_DOWN[0]+UART_SEND_DOWN[1]+UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
            //osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //数据发送 
            //WakeLowLevel(); 
            *outMsgLen=0;
          }break;
          /*------------------------------------------------------------------*/  
        case '4':
          {
            wireless_commnd_flag = 0x34;   //查询锁具状态flag
            
            UART_SEND_DOWN[1]  = 0x17;
            UART_SEND_DOWN[2]  = 0x00;
            UART_SEND_DOWN[3]  = 0x01;
            UART_SEND_DOWN[4]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1] +UART_SEND_DOWN[2]+UART_SEND_DOWN[3];
            
            LEN_UART_SEND_DOWN = 5;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //数据发送 
            //WakeLowLevel();
            *outMsgLen=0;
          }break;
           /*------------------------------------------------------------------*/  
        case '5':
          {
            wireless_commnd_flag = 0x35;   //查询锁具指纹ID号
            
            UART_SEND_DOWN[1]  = 0x1B;
            UART_SEND_DOWN[2]  = 0x00;
            UART_SEND_DOWN[3]  = 0x02;
            UART_SEND_DOWN[4]  = inMsg[2]-'0';
            UART_SEND_DOWN[5]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1] +UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //数据发送 
            //WakeLowLevel();
            *outMsgLen=0;
          }
          break;
          /*------------------------------------------------------------------*/  
         case '6':
          {
            wireless_commnd_flag = 0x36;   //查询锁具卡片ID号
            
            UART_SEND_DOWN[1]  = 0x1C;
            UART_SEND_DOWN[2]  = 0x00;
            UART_SEND_DOWN[3]  = 0x02;
            UART_SEND_DOWN[4]  = inMsg[2]-'0';
            UART_SEND_DOWN[5]  = UART_SEND_DOWN[0]+UART_SEND_DOWN[1] +UART_SEND_DOWN[2]+UART_SEND_DOWN[3]+UART_SEND_DOWN[4];
            
            LEN_UART_SEND_DOWN = 6;
           // osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,OVER_TIME );//2s后应该有串口数据
            osal_start_timerEx( UserApp_TaskID,WAKE_LOW_LEL_EVT,10 ); //数据发送 
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
* @brief   用于处理周期性上报控制命令/C/UPX  /C/USX  /C/UFX 等
*
* @param   uint8 cmd  用于控制周期性上报的命令
* @param   uint8 *inMsg  用于控制周期性上报的数据
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
* @brief   绑定控制处理函数
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
* @brief   指示设设备端点绑定状态
*          绑定成功3s闪烁3次
*          绑定失败3s闪烁6次
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
* @brief   入网成功后处理数据函数
*
* @param   none
*
* @return  none
*/
void UserAppJoinedNetworkCallBack(void)
{
  osal_stop_timerEx(UserApp_TaskID,Check_NetState_EVT);
  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
  
  if(devStartMode == MODE_JOIN)  //首次入网；
  {
    SceneSwitch_Count_Write[0]=0x01;
    osal_nv_item_init(ZCD_NV_SAVE_PARAMETER ,1, NULL);
    osal_nv_write(ZCD_NV_SAVE_PARAMETER ,0,1,&SceneSwitch_Count_Write);//空信息写入NV
    
    osal_start_timerEx(UserApp_TaskID,NET_Message_EVT,10);//2s后应该有串口数据  
  }
  else
  {
    if(Commnd_JoinNet_Flag == 1)
    {
      Commnd_JoinNet_Flag =0;
      osal_start_timerEx(UserApp_TaskID,NET_Message_EVT,10);//2s后应该有串口数据
    }
  }
  
#if defined(LARGER_NETWORK)
  osal_start_timerEx( UserApp_TaskID,USERAPP_COMMAND_SEND_RSSI_MSG_EVT,600000);
#endif
}
/*******************************************************************************
* @fn      UserAppPowerOnJoinNwkDelayCallBack
*
* @brief   设置上电加网延时启动的时间
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
* @brief   启动加网回调函数
*
* @param   uint8 state
*
* @return  none
*/
void UserAppStartNwkJoiningCallBack(void)
{
  osal_nv_item_init(ZCD_NV_MES_FLASH ,1, NULL);
  osal_nv_read(ZCD_NV_MES_FLASH,0,1,&SceneSwitch_Mes_Read);
  if(SceneSwitch_Mes_Read[0] == 0x03) //启动入网操作
  {
    Commnd_JoinNet_Flag=1;
    
    SceneSwitch_Mes_Write[0]=0x00;
    osal_nv_item_init(ZCD_NV_MES_FLASH ,1, NULL);
    osal_nv_write(ZCD_NV_MES_FLASH,0,1,&SceneSwitch_Mes_Write);//空信息写入NV
    
    osal_nv_item_init(ZCD_NV_SAVE_PARAMETER ,1, NULL);
    osal_nv_read(ZCD_NV_SAVE_PARAMETER ,0,1,&SceneSwitch_Count_Read);
    if(SceneSwitch_Count_Read[0]!=1)
    {
      osal_start_timerEx(UserApp_TaskID,Check_NetState_EVT,60000);//一分钟后查询网络状态 
    }  
  }
  SceneSwitch_Mes_Read[0] = 0;
}
/*******************************************************************************
* @fn      UserAppSendHeartBeatCallBack
*
* @brief   发送心跳回调函数,
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
* @brief   通知应用层Coordinator设备是否在网络中，实际检测过程中会有延时，延时时间
*          取决于参数devInfo->MinutesHeartWithApsAck，该接口路由设备有效，
*          EndDevice无效;
*
* @param   offline   如果offline = TRUE;表示Coordinator设备不在网，
*          如果offline = FALSE;表示Coordinator设备在网，
* @return  none
*/
void UserAppCoordinatorOfflineCallback(bool offline)
{
  
}
/*******************************************************************************
* @fn      UserAppJoinNetworkBlinkCallback
*
* @brief   加网闪灯回调函数
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
* @brief   根据信号值强弱闪烁LED灯/以3s钟内指示灯闪烁的次数表现信号值的大小
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
* @brief   用于调光灯具/内嵌式开关的L命令闪灯/响蜂鸣器
*          如果需要启用该功能，在函数UserAppSetDevInfo()中设置
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
* @brief   用户必须设置  iMsg的内容，和iMsg消息的长度 *Len
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
* @brief   设置设备的通用信息
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
* @brief   设置 EndDevice的 PollRate
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
* @brief   产测信道选择
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
* @brief   延时函数
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
  str[0]=sta>>8;//根据设备类型而不同，S表示开关、H表示红外 目前手动改下
  str[1]=sta;
  WZCL_SendDsMessage((uint8*)&str[0],2);//zhouj 061201
}
/*****************************************************************************/
void SampleApp_Message(uint8 Value) //zhouj 061201
{
  uint8 str[2];
  switch(Value)
  {
  case 0x20:     //连续认证失败，系统锁定
    str[0] = 0x02;
    str[1] = 0x1F;
    break;
    
  case 0x23:       //门锁被撬状态上报
    str[0] = 0x02;
    str[1] = 0x1D;
    break;
    
  case 0x25:      //用户添加已满上报提示
    str[0] = 0x02;
    str[1] = 0x1A;
    break;
    
  case 0x3E:      //门锁解除锁定信息上报
    str[0] = 0x02;
    str[1] = 0x1E;
    break;
    
  case 0x30:      //电池欠压状态上报
    str[0] = 0x02;
    str[1] = 0x1C;
    break;
    
  case 0x31:       //反锁上报
    str[0] = 0x02;
    str[1] = 0x0A;
    break;
    
  case 0x32:       //解除反锁
    str[0] = 0x02;
    str[1] = 0x0B;
    break;
    
  case 0x36:        //本地格式化信息上报
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
  case 0x33:         //添加新名单
    str[1] = 0x01; 
    break;
    
  case 0x34:          //本地删除用户信息
    str[1] = 0x02; 
    break;
    
  case 0x35:         //修改客户信息（密码或者时间参数）
    str[1] = 0x03; 
    break;
    
  case 0x37:         //自动删除临时密码
    str[1] = 0x04; 
    break;
    
  case 0x39:        //普通用户信息上传
    str[1] = 0x06; 
    break;
    
  case 0x3A:       //临时用户信息上传
    str[1] = 0x07; 
    break;
    
  case 0x3D:      //本地开锁信息上报（密码、指纹，钥匙）
    str[1] = 0x08; 
    break;
    
  case 0x24:       //密码、指纹劫持上报
    str[1] = 0x09; 
    break;
    
  case 0xAA:       //APP开锁信息上报
    str[1] = 0x0A; 
    break;
    
  case 0x80:      //锁方舌相关状态上报
    str[1] = 0x0B; 
    break;
    
  case 0x3B:      //锁方舌相关状态上报
    str[1] = 0x0C; 
    break;
    
  case 0x51:
    str[1] = 0x0E; //上报无线添加新用户
    break;
    
  case 0x52:
    str[1] = 0x0F; //上报无线删除用户信息
    break;
    
  case 0x53:
    str[1] = 0x0D; //上报修改客户信息
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
   case 0x70: //本地添加指纹ID号上传
     str[1] = 0x01;
     break;
     
   case 0x71: //本地添加卡片ID号上传
     str[1] = 0x02;
     break;
     
    case 0x72: //本地添加指纹ID号上传
     str[1] = 0x03;
     break;
     
   case 0x73: //本地添加卡片ID号上传
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
/************************************分包上传数据************************************************/
void FenBao_Reportup(void)
{
  uint16 i=0;
  dataReportUp[3] = Pkg_cnt; //总包数
  FenBao_cnt++;
  dataReportUp[2] = FenBao_cnt;    //计算分了几次
  
  switch(dataReportUp_flag)
  {
  case 0x01:         //普通用户信息上报
  case 0x03:
    {
      if((Pkg_cnt == 1)&&(Pkg_cnt_flag == 1)) //单包发走
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
        dataReportUp[4]=CommonUserData.TotalUserNum = UART0_RxBuf[4]; //客户总数
        dataReportUp[5]=CommonUserData.PasswordLen =  UART0_RxBuf[5]; //密码长度
        dataReportUp[6]=CommonUserData.UserNum =  UART0_RxBuf[6]; //用户编号
        dataReportUp[7]=CommonUserData.PswGroupNum = 0x01; //密码组数
        dataReportUp[8]=CommonUserData.PswSerialNum = 0x01;//密码ID
        
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
      //整除 || 未整除
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
        dataReportUp[2]=CommonUserEx.CurrentPacket = FenBao_cnt; //当前包数
        dataReportUp[3]=CommonUserEx.TotalPackct   = Pkg_cnt;      //总包数
        dataReportUp[4]=CommonUserEx.TotalUserNum  = UART0_RxBuf[4];//客户总数
        if(FenBao_cnt == 1)
        {
          dataReportUp[5]= CommonUserEx.PasswordLen  = UART0_RxBuf[5];//第一组密码长度
          dataReportUp[6]= CommonUserEx.UserNum      = UART0_RxBuf[6]; //用户编号
          dataReportUp[7]= CommonUserEx.PswGroupNum  = 0x01; //密码组数
          dataReportUp[8]= CommonUserEx.PswSerialNum = 0x01;//密码ID
          
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
          
          dataReportUp[5]=CommonUserEx.PasswordLen  = UART0_RxBuf[CommonUserBuffEx];//第一组密码长度
          dataReportUp[6]=CommonUserEx.UserNum      = UART0_RxBuf[CommonUserBuffEx+1]; //用户编号
          dataReportUp[7]=CommonUserEx.PswGroupNum  = 0x01; //密码组数
          dataReportUp[8]=CommonUserEx.PswSerialNum = 0x01;//密码ID
          
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
      //未整除，最后一包
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
        dataReportUp[2]=CommonUserData.CurrentPacket = FenBao_cnt; //当前包数
        dataReportUp[3]=CommonUserData.TotalPackct =  Pkg_cnt;      //总包数;
        dataReportUp[4]=CommonUserData.TotalUserNum = UART0_RxBuf[4]; //客户总数
        dataReportUp[5]=CommonUserData.PasswordLen = UART0_RxBuf[CommonUserBuffEx]; //密码长度
        dataReportUp[6]=CommonUserData.UserNum =   UART0_RxBuf[CommonUserBuffEx+1]; //用户编号
        dataReportUp[7]=CommonUserData.PswGroupNum = 0x01; //密码组数
        dataReportUp[8]=CommonUserData.PswSerialNum = 0x01;//密码ID
        
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
  case 0x02:        //临时用户相关信息
    {
      if(FenBao_cnt == 1)//单包发走
      {
        dataReportUp[0]=CasualUserData.Head_H = 0x08;
        dataReportUp[1]=CasualUserData.Head_L = 0x07;
        dataReportUp[2]=CasualUserData.CurrentPacket = 0x01;
        dataReportUp[3]=CasualUserData.TotalPackct = Pkg_cnt;
        dataReportUp[4]=CasualUserData.TotalUserNum= UART0_RxBuf[4]; //客户总数
        dataReportUp[5]=CasualUserData.PasswordLen =  UART0_RxBuf[5]; //密码长度
        dataReportUp[6]=CasualUserData.UserNum =  UART0_RxBuf[6]; //用户编号
        dataReportUp[7]=CasualUserData.PswGroupNum = 0x01; //密码组数
        dataReportUp[8]=CasualUserData.PswSerialNum = 0x01;//密码ID
        
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
      else   //不止一包
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
        dataReportUp[4]=CasualUserData.TotalUserNum= UART0_RxBuf[4]; //客户总数
        dataReportUp[5]=CasualUserData.PasswordLen =  UART0_RxBuf[CasualUserBuffEx]; //密码长度
        dataReportUp[6]=CasualUserData.UserNum =  UART0_RxBuf[CasualUserBuffEx+1]; //用户编号
        dataReportUp[7]=CasualUserData.PswGroupNum = 0x01; //密码组数
        dataReportUp[8]=CasualUserData.PswSerialNum = 0x01;//密码ID
        
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
  
  SampleApp_SendDeviceState(0x0F01);//唤醒信号测试
  
  //中断已进入，进行串口自发自收 
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
    SampleApp_SendDeviceState(0x0E01);//串口测试失败
    UART0_RxLength = 0;
  }
  
  if((UART0_RxLength == 8) &&(UART0_RxBuf[0]==0xA1)&&(UART0_RxBuf[1]==0x00)&&
     (UART0_RxBuf[2]==0x01)&&(UART0_RxBuf[3]==0xA2))
  {
    SampleApp_SendDeviceState(0x0F02);//串口测试成功
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
  SampleApp_SendDeviceState(0x0F03);//唤醒输出成功
}
/***************** (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL***********/