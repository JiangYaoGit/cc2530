/**
  ******************************************************************************
  * @file    SampleDeviceUserApp.h
  * @author  Wireless Protocol Stack Develop-Dept.
  * @version 1.0.3
  * @date    2015/12/11,yuzz
  * @brief  This file contains the Sample Device Application definitions.
  *
  * @verbatim
  *
  ******************************************************************************
  * @attention
  *    modified by yuzz   @2016/03/01    E-mail: zhengzheng.yu@wuliangroup.com
  *
  *
  * <h2><center>&copy; COPYRIGHT 2015 WuLianGroup</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SAMPLE_DEVICE_USER_APP_H
#define	SAMPLE_DEVICE_USER_APP_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  uint8  Head_H;
  uint8  Head_L;
  uint8  CurrentPacket;
  uint8  TotalPackct;
  uint8  TotalUserNum;
  uint8  PasswordLen;
  uint8  UserNum;
  bool   PswGroupNum;
  bool   PswSerialNum;
  uint8  PswTime[12];
}CasualUserData_t;

typedef struct
{
  uint8  Head_H;
  uint8  Head_L;
  uint8  CurrentPacket;
  uint8  TotalPackct;
  uint8  TotalUserNum;
  uint8  PasswordLen;
  uint8  UserNum;
  bool   PswGroupNum;
  bool   PswSerialNum;
}CommonUserData_t;

typedef struct
{
  uint8  Head_H;
  uint8  Head_L;
  uint8  CurrentPacket;
  uint8  TotalPackct;
  uint8  TotalUserNum;
  uint8  PasswordLen;
  uint8  UserNum;
  bool   PswGroupNum;
  bool   PswSerialNum;
  uint8  PasswordLenEx;
  uint8  UserNumEx;
  bool   PswGroupNumEx;
  bool   PswSerialNumEx;
}CommonUserEx_t;


/* Includes ------------------------------------------------------------------*/
#include "ZComDef.h"
#include "wzcl.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
// These constants are only for example and should be changed to the
// device's needs
#define USERAPP_MAX_ONOFF_ENDPOINTS                 0x06
#define USERAPP_ENDPOINT1                           WZCL_ENDPOINT1
#define USERAPP_ENDPOINT2                           WZCL_ENDPOINT2
#define USERAPP_ENDPOINT3                           WZCL_ENDPOINT3
#define USERAPP_ENDPOINT4                           WZCL_ENDPOINT4
#define USERAPP_ENDPOINT5                           WZCL_ENDPOINT5
#define USERAPP_ENDPOINT6                           WZCL_ENDPOINT6
//the Clusters for Device User App  definitions
#define USERAPP_CLUSTER_ID_GEN_ONOFF                WZCL_CLUSTER_ID_GEN_ONOFF
#define USERAPP_CLUSTER_ID_GEN_DIMMER               WZCL_CLUSTER_ID_GEN_DIMMER
#define USERAPP_CLUSTER_ID_GEN_IR                   WZCL_CLUSTER_ID_GEN_IR
#define USERAPP_CLUSTER_ID_GEN_CERTAIN_CONTROL      WZCL_CLUSTER_ID_GEN_CERTAIN_CONTROL
#define USERAPP_CLUSTER_ID_GEN_ELOCK                WZCL_CLUSTER_ID_GEN_ELOCK
#define USERAPP_CLUSTER_ID_GEN_HUE                  WZCL_CLUSTER_ID_GEN_HUE

// Application Events (OSAL) - These are bit weighted definitions.
#define  Start_Joinnet_EVT                      0x0004
#define  WAKE_LOW_LEL_EVT                       0x0002
#define  UART_RX_EVT                            0x0001
#define  UART_TX2_EVT                           0x0008


#define  WAKE_UP_OUT_EVT                        0x0010
#define  Check_NetState_EVT                     0x0020
#define  OVERTIME_EVT                           0x0040
#define  FenBao_Report_EVT                      0x0080

#define  USERAPP_COMMAND_SEND_RSSI_MSG_EVT      0x0100
#define  NET_Message_EVT                        0x0200
#define  Auto_Lock_EVT                          0x0400
#define  Leave_Network_EVT                      0x0800

#define  SET_POLLTIME_EVT                       0x1000

// Send Message Timeout
#define USERAPP_SEND_PERIODIC_MSG_TIMEOUT            5000     // Every 5 seconds

// Group ID for Flash Command
#define USERAPP_GROUP1                       	    0x0001
#define USERAPP_GROUP2                       	    0x0002
#define USERAPP_GROUP3                       	    0x0003
#define USERAPP_GROUP4                       	    0x0004
#define USERAPP_GROUP5                       	    0x0005
#define USERAPP_GROUP6                       	    0x0006
#define USERAPP_GROUP7                       	    0x0007
#define USERAPP_GROUP8                       	    0x0008
#define USERAPP_GROUP9                       	    0x0009
#define USERAPP_GROUP10                      	    0x000A

#define DEFAULT_CHANLIST_26                         0x04000000  // 26 - 0x1A
#define DEFAULT_CHANLIST_25                         0x02000000  // 25 - 0x19
#define DEFAULT_CHANLIST_24                         0x01000000  // 24 - 0x18
#define DEFAULT_CHANLIST_23                         0x00800000  // 23 - 0x17
#define DEFAULT_CHANLIST_22                         0x00400000  // 22 - 0x16
#define DEFAULT_CHANLIST_21                         0x00200000  // 21 - 0x15
#define DEFAULT_CHANLIST_20                         0x00100000  // 20 - 0x14
#define DEFAULT_CHANLIST_19                         0x00080000  // 19 - 0x13
#define DEFAULT_CHANLIST_18                         0x00040000  // 18 - 0x12
#define DEFAULT_CHANLIST_17                         0x00020000  // 17 - 0x11
#define DEFAULT_CHANLIST_16                         0x00010000  // 16 - 0x10
#define DEFAULT_CHANLIST_15                         0x00008000  // 15 - 0x0F
#define DEFAULT_CHANLIST_14                         0x00004000  // 14 - 0x0E
#define DEFAULT_CHANLIST_13                         0x00002000  // 13 - 0x0D
#define DEFAULT_CHANLIST_12                         0x00001000  // 12 - 0x0C
#define DEFAULT_CHANLIST_11                         0x00000800  // 11 - 0x0B
// NV Items Reserved for applications (user applications)
// 0x0501 ?0x0FFF
#define ZCD_NV_SAVE_PARAMETER                   	0x0512
#define ZCD_NV_MES_FLASH                                0x0501
/* Exported macro ------------------------------------------------------------*/
#define UserApp_epDesc1 wzcl_epDesc1
#define UserApp_epDesc2 wzcl_epDesc2
#define UserApp_epDesc3 wzcl_epDesc3
#define UserApp_epDesc4 wzcl_epDesc4
#define UserApp_epDesc5 wzcl_epDesc5
#define UserApp_epDesc6 wzcl_epDesc6
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* SAMPLE_DEVICE_USER_APP_H */
/******************* (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL*********/