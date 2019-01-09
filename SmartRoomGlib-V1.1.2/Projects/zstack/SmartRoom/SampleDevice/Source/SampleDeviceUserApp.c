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
/*******************************************************************************
 * LOCAL CONSTANTS
 */

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
  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
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
//  HalLedBlink ( HAL_LED_1, 1, 50, 500 );
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
  rsMsg[0] = 0;
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
  devInfo->devId = 2;
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
    pollrate->shortPollInterval = 5000;
    pollrate->enablelongPoll = 1;
    pollrate->longPollInterval = 18000;
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
/***************** (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL***********/