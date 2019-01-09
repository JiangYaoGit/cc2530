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
#include <stdio.h>
#include <stdlib.h>
#include "AF.h"
#include "OnBoard.h"
#include "ZDObject.h"
#include "hal_led.h"
#include "hal_key.h"
#include "DeviceConfig.h"
/*******************************************************************************
 * LOCAL CONSTANTS
 */
#include "DimmerDrive.h"
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void UserAppProcessKeyNwkAction(void);
static void UserAppHandleKeys( uint8 shift, uint16 keys );
static void UserAppProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
/*******************************************************************************
 * GLOBAL VARIABLES
 */
const uint8 FWVersion   = 0x0A;
const uint8 FWDate[] = (__DATE__);
const uint8 DisableManufacture = TRUE;
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
 * USER VARIABLES
 */
uint8 LeaveNWKCnt = 0;          //退网面板灯闪计数
volatile uint8 NWKStateFlag = 0;         //网络状态标志位
uint8 LeaveNWKBack[11] = {0xFA,0x08,0x12,0x33,0x00,0x00,0xFF,0xFE,0xFF,0xFE,0x41}; //退网灯闪指令
uint8 RetainNwk[2] = {1,100};       //记录APP下发的数据
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
  
  Uart0Init();
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
  uint8 DecToString[4] = {0};
  uint16 StringToHex = 0;
  uint8 SendBuf[2] = {0};
  uint8 ExchangeData = 0; 
    
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
  if(events & UART0_RX_EVT)
  {
    /* ...MCU上传数据... */
    //测试接受长度-单位（字节）
    //WZCL_SendDsMessage(UART0RxBuf,UART0RxBufLen);
    //WZCL_SendDsMessage(&UART0RxBufLen,1);
    UART0RxBufLen = 0;
   
     //控制返回一路处理
    if(UART0RxBuf[2] == 0x14 && UART0RxBuf[3] == 0x02)
    {
      ExchangeData = HexToDec(UART0RxBuf[11]);    
      
      if(ExchangeData == 0)
      {
        RetainNwk[0] = 1; 
      }
      else 
      {
        RetainNwk[0] = 2; //实现调到一百
        RetainNwk[1] = ExchangeData;
      }
      
      sprintf(DecToString, "%d", ExchangeData);
      
      StringToHex = strtol(DecToString, 0, 16);
      
      SendBuf[0] = StringToHex >> 8;
      SendBuf[1] = StringToHex;
      WZCL_SendDsMessage(SendBuf,2);
    }
    //退网申请  
    else if(UART0RxBuf[1] == 0x0C && UART0RxBuf[2] == 0x12 && UART0RxBuf[3] == 0x24) 
    {
      LeaveNWKCnt = 20;
      //可以进入，实现退网   退网后如何自动加网？
      //WZCL_SendDsMessage(UART0RxBuf,15);
      WZCL_LeaveAndRestoreFactory();
    }
    
    return events ^ UART0_RX_EVT;
  }
  if(events & NWK_BACK_EVT)
  {

    /* ...面板退网申请，反馈事件,灯闪20下... */
    Uart0Sent(LeaveNWKBack,11);
    
    return events ^ NWK_BACK_EVT;
  }
  if(events & NWK_AUTO_EVT)
  {

    /* ..自动加网实现.... */
    WZCL_FindAndJoinNetwork();
    
    return events ^ NWK_AUTO_EVT;
  }
  if(events & USERAPP_EVENT_EVT)
  {

    /* ...... */

    return events ^ USERAPP_EVENT_EVT;
  }
  /* ...... */
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
    if((devState == DEV_END_DEVICE) || (devState == DEV_ROUTER)){
      HalLedBlink ( HAL_LED_1, 1, 50, 1000 );
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
      //测试点Uart0Sent("888",4);
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
  {
    NWKStateFlag = 0;
    
    //测试点Uart0Sent("777",4);
    HalLedBlink ( HAL_LED_1, 4, 50, 1000 );
  }
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
  //测试接受长度-单位（字节）
  //Uart0Sent(UART0TxBuf,inMsgLen);
  //Uart0Sent(&inMsgLen,1);
  
  uint8 var[3] = {0};
  uint8 ExchangeData = 0; 
  var[2] = *inMsg;       //高位0-1
  var[1] = *(inMsg + 1);
  var[0] = *(inMsg + 2);//低位0-9
  if(var[2] == '1'|| var[2] == '0')
  {
    if(var[1] >= '0' && var[1] <= '9' && var[0] >= '0' && var[0] <= '9')
    {
      var[2] %= 16;
      var[1] %= 16;
      var[0] %= 16;       
      ExchangeData = var[2]*100 + var[1]*10 + var[0];
      
      //APP开状态和调到100的区别
      if(ExchangeData == 100)
      {
        //APP引起的OFF和设备引起的OFF的区别
        if(RetainNwk[0] == 1)
        {
          RetainNwk[0] = 0;
          ExchangeData = RetainNwk[1];
        }
      }
      if(ExchangeData != 0)
        RetainNwk[1] = ExchangeData; //保留当前app下发的数据
        
      
      UART0TxBuf[11] = DecToHex(ExchangeData);
      
            //检验和
      UART0TxBuf[12] = UART0TxBuf[11] + UART0TxBuf[10]+ UART0TxBuf[9]+ UART0TxBuf[8]
        + UART0TxBuf[7]+ UART0TxBuf[6]+ UART0TxBuf[5]+ UART0TxBuf[4]+ UART0TxBuf[3]
          + UART0TxBuf[2]+ UART0TxBuf[1]+ UART0TxBuf[0];
      
      Uart0Sent(UART0TxBuf,13);
    }
  }
  
  *outMsgLen = 0;
  //Uart0Sent(UART0TxBuf,17);
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
  NWKStateFlag = 1; 
  
  //测试点Uart0Sent("666",4);
  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
  
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
  rsMsg[0] = 0;
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
  devInfo->ctrlChar = 'D';
  devInfo->cmdIndCh = '/';
  devInfo->devId = 12;
  devInfo->rsLen = 1;
  /* if you want create a binding link ,set the parameter*/
  devInfo->extEpNums = 0;
  devInfo->lowBatteryAlarm.EnableAlarm = 1;
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
  return TRUE;  
  //return FALSE;
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
/***************** (C) COPYRIGHT 2017 WuLianGroup ********END OF FIL***********/