/**
  ******************************************************************************
  * @file    SampleDeviceUserApp.c
  * @author  Wireless Protocol Stack Develop-Dept.
  * @version 1.1.3
  * @date    2017/01/18,yuzz
  * @brief  Sample Device Application (no Profile).
  *
  * @verbatim
  *
  ******************************************************************************
  * @attention
  *    modified by yuzz   @2017/01/18  E-mail: zhengzheng.yu@wuliangroup.com
  *
  *
  * <h2><center>&copy; COPYRIGHT 2017 WuLianGroup</center></h2>
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
#include "AOKMotorUart.h"
#include "AOKService.h"
/*******************************************************************************
 * LOCAL CONSTANTS
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void UserAppProcessKeyNwkAction(void);
static void UserAppHandleKeys( uint8 shift, uint16 keys );
static void UserAppProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
extern void Close_Relay_Safe(void);
/*******************************************************************************
 * GLOBAL VARIABLES
 */
const uint8 FWVersion= 0x0A;
const uint8 FWDate[] = (__DATE__);
const uint8 DisableManufacture = TRUE;
const uint8 License[10] = {0x64,0x27,0xC6,0xA8,0x66,0xFC,0x10,0x07,0x1C,0x1E};
/*******************************************************************************
 * LOCAL VARIABLES
 */
const cId_t bindingInClusterList[USERAPP_IN_BINDINGLIST]=
{
  USERAPP_CLUSTER_ID_GEN_CERTAIN_CONTROL
};

const cId_t bindingOutClusterList[USERAPP_OUT_BINDINGLIST]=
{
  USERAPP_CLUSTER_ID_GEN_CERTAIN_CONTROL
};

uint8 UserApp_TaskID;
bool BlinkLedForRssi = FALSE;
DUYAMOTOR Motor;
uint8 CommndFlag;

/*******************************************************************************
 * USER VARIABLES
 */
uint8 LEN_UART_SEND_DOWN;
uint8 UART_SEND_DOWN[10]={0x55,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//下发的控制命令

void BingdingSuccessReponse(void);
void BingdingFailReponse(void);
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
  ZDO_RegisterForZDOMsg( UserApp_TaskID, End_Device_Bind_rsp );
  hal_USART0Init();
  osal_start_timerEx( UserApp_TaskID,SAFE_CLOSE_RELAY,90000);//一分半钟以后强制关闭继电器
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
  /****************************************************************************/  
  if(events & UART_RX_COMPLETED)
  {
    M0042_uartRXFrameAnalysis();
    UART_Buf_Len=0;
    osal_memset(UART_RxBuf,0,40);
    return(events ^ UART_RX_COMPLETED);
  }
  /****************************************************************************/
  if(events & JOIN_NET_OK_EVT )
  {
    JoinNetReponseoK();
    return (events ^ JOIN_NET_OK_EVT );  
  }
  /****************************************************************************/
  if(events & GET_MOTOR_STATUS_EVT)
  {
    GetMotorStatus_Event();
    return (events ^ GET_MOTOR_STATUS_EVT);   
  }
  if(events & SAFE_CLOSE_RELAY)
  {
    Close_Relay_Safe();
    return (events ^ SAFE_CLOSE_RELAY);
  }
  return 0;
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
      HalLedBlink ( HAL_LED_1, 1, 50, 1000 );
  }
#ifndef HOLD_AUTO_START
  else
  {
    WZCL_FindAndJoinNetwork();
  }
#endif
  //}
  else if(KeyMultiClicks == 2)
  {
    if( (devState==DEV_END_DEVICE) || (devState==DEV_ROUTER) )
    {
      WZCL_InterRssiReqMessage();
      BlinkLedForRssi = TRUE;
    }
  }
  else if(KeyMultiClicks == 4)  //手动申请入网
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
    LeaveNetResponse();
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
  switch(inMsg[0])
  {
  case NoTripCommnd:  //无行程比
    CommndFlag = 1;
    Motor.Uart_ASk_Flag = 0x04;//无行程比命令
    if((inMsg[1]=='0') || (inMsg[1]=='1'))
    {
      if((inMsg[1]=='0')&&(inMsg[2]=='0')&&(inMsg[3]=='0')) //关闭
      {
        UART_SEND_DOWN[3] = 0x03;
        UART_SEND_DOWN[4] = 0x02;
      }
      if((inMsg[1]=='1')&&(inMsg[2]=='0')&&(inMsg[3]=='0')) //开启
      {
        UART_SEND_DOWN[3] = 0x03;
        UART_SEND_DOWN[4] = 0x01;
      } 
      CRC16((uint8 *)UART_SEND_DOWN,5); //求校验值
      UART_SEND_DOWN[5] = Motor.chCRCLo;
      UART_SEND_DOWN[6] = Motor.chCRCHi;
      LEN_UART_SEND_DOWN = 7;
    }
    /*-----------------------------------------------------------------------*/
    if((inMsg[1]=='2')&&(inMsg[2]=='5')&&(inMsg[3]=='5'))
    {
      UART_SEND_DOWN[3] = 0x03;
      UART_SEND_DOWN[4] = 0x03; 
      UART_SEND_DOWN[5] = 0x01;
      CRC16((uint8 *)UART_SEND_DOWN,6); //求校验值
      UART_SEND_DOWN[6] = Motor.chCRCLo;
      UART_SEND_DOWN[7] = Motor.chCRCHi;
      LEN_UART_SEND_DOWN = 8; 
    }
    hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
    *outMsgLen=0;
    break;
/******************************************************************************/    
  case TripCommnd: //行程比
    CommndFlag = 2;
    UART_SEND_DOWN[3] = 0x03;
    UART_SEND_DOWN[4] = 0x04;
    UART_SEND_DOWN[5] = (inMsg[1]-'0')*100+(inMsg[2]-'0')*10+(inMsg[3]-'0');
    CRC16((uint8 *)UART_SEND_DOWN,6); //求校验值
    UART_SEND_DOWN[6] = Motor.chCRCLo;
    UART_SEND_DOWN[7] = Motor.chCRCHi;
    /*------------------------------------------------------------------------*/
    if(UART_SEND_DOWN[5] == 0xFF)   //行程比状态下发送停止命令
    {
      Motor.Uart_ASk_Flag = 0x03;   //表示有行程的情况下，停止命令
      UART_SEND_DOWN[3] = 0x03;
      UART_SEND_DOWN[4] = 0x03; 
      UART_SEND_DOWN[5] = 0x01;
      CRC16((uint8 *)UART_SEND_DOWN,6); //求校验值
      UART_SEND_DOWN[6] = Motor.chCRCLo;
      UART_SEND_DOWN[7] = Motor.chCRCHi;
    }
    else
    {
      Motor.Uart_ASk_Flag = 0x03;   //表示有行程比命令
    }
    LEN_UART_SEND_DOWN = 8;
    hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
    *outMsgLen=0;
    break;
/******************************************************************************/  
  case 0x32:
    {
      static uint8 str[9]={0x55,0xFE,0xFE,0x03,0x03,0x01,0x25,0x12};
      hal_UARTWrite(str,8);
    }break;
  case ReversCommnd: //换向命令
    UART_SEND_DOWN[3] = 0x02;
    UART_SEND_DOWN[4] = 0x03; 
    UART_SEND_DOWN[5] = 0x01;
    UART_SEND_DOWN[6] = (inMsg[1]-'0')*10+(inMsg[2]-'0');
    CRC16((uint8 *)UART_SEND_DOWN,7); //求校验值
    UART_SEND_DOWN[7] = Motor.chCRCLo;
    UART_SEND_DOWN[8] = Motor.chCRCHi;
    LEN_UART_SEND_DOWN = 9;
    hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
    *outMsgLen=0;
    break;
/******************************************************************************/      
  case EmptyTripCommnd://清行程命令
    UART_SEND_DOWN[3] = 0x03; 
    UART_SEND_DOWN[4] = 0x07;
    CRC16((uint8 *)UART_SEND_DOWN,5); //求校验值
    UART_SEND_DOWN[5] = Motor.chCRCLo;
    UART_SEND_DOWN[6] = Motor.chCRCHi;
    LEN_UART_SEND_DOWN = 7;
    hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
    *outMsgLen=0;
    break;
/******************************************************************************/      
  default:
    break;
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
    if(pkt->cmd.Data[0]==1)  //停
    {
      UART_SEND_DOWN[3] = 0x03;
      UART_SEND_DOWN[4] = 0x03;
      UART_SEND_DOWN[5] = 0x01;
      UART_SEND_DOWN[6] = 0x25;
      UART_SEND_DOWN[7] = 0x12;
      LEN_UART_SEND_DOWN = 8;
    }
    else if(pkt->cmd.Data[0]==2) //开
    {
      UART_SEND_DOWN[3] = 0x03;
      UART_SEND_DOWN[4] = 0x01;
      UART_SEND_DOWN[5] = 0xb9;
      UART_SEND_DOWN[6] = 0x24;
      LEN_UART_SEND_DOWN = 7;
    }
    else if(pkt->cmd.Data[0]==3) //关
    {
      UART_SEND_DOWN[3] = 0x03;
      UART_SEND_DOWN[4] = 0x02;
      UART_SEND_DOWN[5] = 0xf9;
      UART_SEND_DOWN[6] = 0x25;
      LEN_UART_SEND_DOWN = 7;
    }
    hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN); 
    break;
    
  case USERAPP_ENDPOINT2:
    break;
    
  case USERAPP_ENDPOINT3:
    break;

  default:
    break;
  }
}

/*****************************************************************************
* Function Name	  :  BingdingSuccessReponse
* Description	  :  绑定成功
* Input 	  :  None
* Output	  :  None
* Return	  :  None
*******************************************************************************/
void BingdingSuccessReponse(void)
{
  UART_SEND_DOWN[3] = 0x07;
  UART_SEND_DOWN[4] = 0x03;
  
  CRC16((uint8 *)UART_SEND_DOWN,5); //求校验值
  UART_SEND_DOWN[5] = Motor.chCRCLo;
  UART_SEND_DOWN[6] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 7;
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}

/*****************************************************************************
* Function Name	  :  BingdingFailReponse
* Description	  :  绑定失败
* Input 	  :  None
* Output	  :  None
* Return	  :  None
*******************************************************************************/
void BingdingFailReponse(void)
{
  UART_SEND_DOWN[3] = 0x07;
  UART_SEND_DOWN[4] = 0xFF;
  
  CRC16((uint8 *)UART_SEND_DOWN,5); //求校验值
  UART_SEND_DOWN[5] = Motor.chCRCLo;
  UART_SEND_DOWN[6] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 7;
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
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
      BingdingSuccessReponse();
      //HalLedBlink ( HAL_LED_1, 3, 50, 1000 );
    }
    else
    {
      BingdingFailReponse();
      //HalLedBlink ( HAL_LED_1, 6, 50, 500 );
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
  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
//  if(devStartMode == MODE_JOIN)
//  {
  QueryMotorStroke();
  osal_start_timerEx( UserApp_TaskID,JOIN_NET_OK_EVT,1000); 
//  }
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
//  HalLedBlink ( HAL_LED_1, 1, 50, 500 );
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
  rsMsg[0] = dataReportUp[0];
  rsMsg[1] = dataReportUp[1] ;
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
  devInfo->ctrlChar = 'h';
  devInfo->cmdIndCh = '/';
  devInfo->devId = 80;
  devInfo->rsLen = 2;
  /* if you want create a binding link ,set the parameter*/
  devInfo->extEpNums = 1;
  devInfo->lowBatteryAlarm.EnableAlarm = 0;
  devInfo->lowBatteryAlarm.DetectMode = MODE_HOUR;
#if !defined(RTR_NWK)
  devInfo->maxPollFailureRetries = 10;
#endif
  devInfo->MinutesSendHeartToCoord = 10;

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
    pollrate->shortPollInterval = 5000;
    pollrate->enablelongPoll = 1;
    pollrate->longPollInterval = 18000;
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
  return ChannelList;
}


/*******************************************************************************
 * @fn      UserAppBindValidCheck
 *
 * @brief   
 *
 * @param   none
 * @param   none
 *
 * @return  none
 */
uint8 UserAppBindValidCheck(uint8 EndpointId)
{
  uint8 BindValid = 0;

  if(bindNumReflections(EndpointId,USERAPP_CLUSTER_ID_GEN_ONOFF)>0)
  {
    BindValid = 1;
  }

  if(bindNumReflections(EndpointId,USERAPP_CLUSTER_ID_GEN_DIMMER)>0)
  {
    BindValid = 1;
  }

  if(bindNumReflections(EndpointId,USERAPP_CLUSTER_ID_GEN_CERTAIN_CONTROL)>0)
  {
    BindValid = 1;
  }

  if(bindNumReflections(EndpointId,USERAPP_CLUSTER_ID_GEN_ELOCK)>0)
  {
    BindValid = 1;
  }

  if(bindNumReflections(EndpointId,USERAPP_CLUSTER_ID_GEN_IR)>0)
  {
    BindValid = 1;
  }

   if(bindNumReflections(EndpointId,USERAPP_CLUSTER_ID_GEN_HUE)>0)
  {
    BindValid = 1;
  }

  return BindValid;
}

void UserAppProcessZdpBindUnbindRspMsgsCallBack(zAddrType_t *dstAddr,uint16 cmd, uint8 status)
{
#if defined(BLINK_LEDS)
  if(( cmd == Bind_rsp ) || ( cmd == Unbind_rsp ))
  {
    if((status == ZDP_NOT_SUPPORTED) || ( status == ZDP_INVALID_EP ))
    {
      HalLedBlink ( HAL_LED_1, 6, 50, 1000 );
    }
    else
    {
      if ( cmd == Bind_rsp )
      {
        if(status == ZDP_SUCCESS)
        {
          HalLedBlink ( HAL_LED_1, 3, 50, 1000 );
        }
        else if(status == ZDP_TABLE_FULL)
        {
          HalLedBlink ( HAL_LED_1, 6, 50, 1000 );
        }
        else
        {
          HalLedBlink ( HAL_LED_1, 6, 50, 1000 );
        }
      }
      else// Unbind_rsp
      {
        if(status == ZDP_SUCCESS)
        {
          HalLedBlink ( HAL_LED_1, 3, 50, 1000 );
        }
        else if(status == ZDP_NO_ENTRY)
        {
          HalLedBlink ( HAL_LED_1, 6, 50, 1000 );
        }
        else
        {
          HalLedBlink ( HAL_LED_1, 6, 50, 1000 );
        }
      }
    }
  }
#endif
}


/***************** (C) COPYRIGHT 2017 WuLianGroup ********END OF FIL***********/