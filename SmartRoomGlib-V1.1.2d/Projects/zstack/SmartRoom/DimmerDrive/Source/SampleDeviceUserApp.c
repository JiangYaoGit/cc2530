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
uint8 LeaveNWKCnt = 0;          //��������������
volatile uint8 NWKStateFlag = 0;         //����״̬��־λ
uint8 LeaveNWKBack[11] = {0xFA,0x08,0x12,0x33,0x00,0x00,0xFF,0xFE,0xFF,0xFE,0x41}; //��������ָ��
uint8 RetainNwk[2] = {1,100};       //��¼APP�·�������
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
    }//end�� while ( MSGpkt )

    return (events ^ SYS_EVENT_MSG);
  }
  if(events & UART0_RX_EVT)
  {
    /* ...MCU�ϴ�����... */
    //���Խ��ܳ���-��λ���ֽڣ�
    //WZCL_SendDsMessage(UART0RxBuf,UART0RxBufLen);
    //WZCL_SendDsMessage(&UART0RxBufLen,1);
    UART0RxBufLen = 0;
   
     //���Ʒ���һ·����
    if(UART0RxBuf[2] == 0x14 && UART0RxBuf[3] == 0x02)
    {
      ExchangeData = HexToDec(UART0RxBuf[11]);    
      
      if(ExchangeData == 0)
      {
        RetainNwk[0] = 1; 
      }
      else 
      {
        RetainNwk[0] = 2; //ʵ�ֵ���һ��
        RetainNwk[1] = ExchangeData;
      }
      
      sprintf(DecToString, "%d", ExchangeData);
      
      StringToHex = strtol(DecToString, 0, 16);
      
      SendBuf[0] = StringToHex >> 8;
      SendBuf[1] = StringToHex;
      WZCL_SendDsMessage(SendBuf,2);
    }
    //��������  
    else if(UART0RxBuf[1] == 0x0C && UART0RxBuf[2] == 0x12 && UART0RxBuf[3] == 0x24) 
    {
      LeaveNWKCnt = 20;
      //���Խ��룬ʵ������   ����������Զ�������
      //WZCL_SendDsMessage(UART0RxBuf,15);
      WZCL_LeaveAndRestoreFactory();
    }
    
    return events ^ UART0_RX_EVT;
  }
  if(events & NWK_BACK_EVT)
  {

    /* ...����������룬�����¼�,����20��... */
    Uart0Sent(LeaveNWKBack,11);
    
    return events ^ NWK_BACK_EVT;
  }
  if(events & NWK_AUTO_EVT)
  {

    /* ..�Զ�����ʵ��.... */
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
      //���Ե�Uart0Sent("888",4);
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
  {
    NWKStateFlag = 0;
    
    //���Ե�Uart0Sent("777",4);
    HalLedBlink ( HAL_LED_1, 4, 50, 1000 );
  }
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
  //���Խ��ܳ���-��λ���ֽڣ�
  //Uart0Sent(UART0TxBuf,inMsgLen);
  //Uart0Sent(&inMsgLen,1);
  
  uint8 var[3] = {0};
  uint8 ExchangeData = 0; 
  var[2] = *inMsg;       //��λ0-1
  var[1] = *(inMsg + 1);
  var[0] = *(inMsg + 2);//��λ0-9
  if(var[2] == '1'|| var[2] == '0')
  {
    if(var[1] >= '0' && var[1] <= '9' && var[0] >= '0' && var[0] <= '9')
    {
      var[2] %= 16;
      var[1] %= 16;
      var[0] %= 16;       
      ExchangeData = var[2]*100 + var[1]*10 + var[0];
      
      //APP��״̬�͵���100������
      if(ExchangeData == 100)
      {
        //APP�����OFF���豸�����OFF������
        if(RetainNwk[0] == 1)
        {
          RetainNwk[0] = 0;
          ExchangeData = RetainNwk[1];
        }
      }
      if(ExchangeData != 0)
        RetainNwk[1] = ExchangeData; //������ǰapp�·�������
        
      
      UART0TxBuf[11] = DecToHex(ExchangeData);
      
            //�����
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
  NWKStateFlag = 1; 
  
  //���Ե�Uart0Sent("666",4);
  HalLedBlink ( HAL_LED_1, 1, 99, 2000 );
  
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