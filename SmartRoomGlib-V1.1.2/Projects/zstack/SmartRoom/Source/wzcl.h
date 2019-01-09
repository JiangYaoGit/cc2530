/**************************************************************************************************
  Filename:       wzcl.h
  Revised:        $Date: 2015-08-10 $
  Revision:       $Revision: 0002 $

  Description:    This file contains the Wulian Zigbee Cluster Library Foundation functions.

**************************************************************************************************/

#ifndef WZCL_H
#define WZCL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "AF.h"
#include "aps_groups.h"

#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OSAL_Tasks.h"

/*********************************************************************
 * CONSTANTS
 */
// device's needs
#define WZCL_ENDPOINT                               20
#define WZCL_ENDPOINT1                              21
#define WZCL_ENDPOINT2                              22
#define WZCL_ENDPOINT3                              23
#define WZCL_ENDPOINT4                              24
#define WZCL_ENDPOINT5                              25
#define WZCL_ENDPOINT6                              26
#define WZCL_PROFID                                 0x0F08
#define WZCL_DEVICEID                               0x0001
#define WZCL_DEVICE_VERSION                         0
#define WZCL_FLAGS                                  0

// General Clusters
#define WZCL_CLUSTER_ID_GEN_BASIC                   0x0000//设备基本信息专用
#define WZCL_CLUSTER_ID_GEN_ONOFF                   0x0001//开关绑定专用
#define WZCL_CLUSTER_ID_GEN_ACK                     0x0002
#define WZCL_CLUSTER_ID_GEN_ANNOUNCE                0x0003//设备上线声明OS专用CLuster
#define WZCL_CLUSTER_ID_GEN_CMD                     0x0004//
#define WZCL_CLUSTER_ID_ROUTER_HEART_BEAT           0x0005
#define WZCL_CLUSTER_ID_GEN_ANNCE_REQ               0x0006
#define WZCL_CLUSTER_ID_COORDINATOR_HEART_BEAT      0x0007
#define WZCL_CLUSTER_ID_ACTIVE_COMMAND              0x0008 //主动上报命令
#define WZCL_CLUSTER_ID_RSSI_CHECK                  0x000A

#define WZCL_CLUSTER_ID_GEN_DIMMER                  0x000B
#define WZCL_CLUSTER_ID_GEN_IR                      0x000C
#define WZCL_CLUSTER_ID_GEN_CERTAIN_CONTROL         0x000D
#define WZCL_CLUSTER_ID_GEN_ELOCK                   0x000E
#define WZCL_CLUSTER_ID_GEN_HUE                     0x000F

#define WZCL_MAX_CLUSTERS                           0x09
#define WZCL_MAX_BINDING_CLUSTERS                   0x07
// WZCL Events (OSAL) - These are bit weighted definitions.

#define WZCL_SEND_MSG_DELAY_EVT                     0x0001
#define WZCL_DEVICE_JOINROLE_EVT                    0x0002
#define WZCL_DEVICE_RESTORE_EVT                     0x0004
#define WZCL_DEVICE_ASSOCIA_EVT                     0x0008
#define WZCL_DEVICE_OSACK_EVT                       0x0010
#define WZCL_DEVICE_ZB_ENTRY_EVT                    0x0020/* 设备入网入口*/
#define WZCL_POLL_CONTROL_EVT                       0x0040
#define WZCL_CHECK_LOWBUTTERY_EVT                   0x0080

#define WZCL_PERMIT_JOIN_EVT                        0x0100
#define WZCL_SEND_DEVICE_FWMSG_EVT                  0x0200
#define WZCL_INTER_SIG_CHECK_EVT                    0x0400
#define WZCL_SEND_DEVICE_HEART_EXTRA_EVT            0x0800
#define WZCL_SEND_DEVICE_HEART_EVT                  0x1000

#define WZCL_SEND_DEVICE_ANNCE_EVT                  0x2000
#define WZCL_SEND_DEVICE_RSMSG_EVT                  0x4000


#define WZCL_ZB_ENTRY_DELAY_TIMEOUT                 1000/* 设备入网入口延时*/
#define WZCL_ZB_RESTORE_TIMEOUT                     1000

// Device type
#define WZCL_DEVICETYPE_ROUTER           0x00
#define WZCL_DEVICETYPE_ENDDEVICE        0x01
/*********************************************************************
 * MACROS
 */


#define ICMODEL_CC2530_NA                          0x00
#define ICMODEL_CC2530_2591                        0x01
#define ICMODEL_CC2530_2401C                       0x02
#define ICMODEL_CC2538_NA                          0x03
#define ICMODEL_CC2538_2592                        0x04
#define ICMODEL_CC2538_2401C                       0x05

// Device Command type
#define WZCL_DEVICE_GENERAL_CMD                    0x43//通用设备命令        C
#define WZCL_DEVICE_ASSOCILIST_CMD                 0x44//设备关联信息命令    D
#define WZCL_DEVICE_GATEWAY_CMD                    0x44//网关设备命令        D
// Device General Command type
#define WZCL_DEVICE_GENERAL_OPERATE_CMD            0x55//通用操作命令        U
#define WZCL_DEVICE_GENERAL_BINDING_CMD            0x44//绑定控制命令        D
#define WZCL_DEVICE_PERMIT_JOINTYPE_CMD            0x41//允许加入设备类型    A
//Device Operate Command
#define REFRESH_DEVICE_INFO_CMD                    0x52//刷新设备            R
#define ASSOCILIST_REQ_CMD                         0x61//请求设备关联表      a
#define DEVICE_RSSI_REQ_CMD                        0x61//请求设备RSSI        f




#define WZCL_REPORT_ASSOCIATE_CMD                   0x46
#define WZCL_NWK_LOSS_MSG_CMD                       0x47
#define WZCL_REPORT_ASSOCIATE_ID                    0x55AA
#define WZCL_NWK_LOSS_MSG_ID                        0xAA55
/*********************************************************************
 * TYPEDEFS
 */
/* WZCL Indication Callback Registration */
typedef void* (*pfnWzclCb)( void *param );

#ifdef ewarm
#pragma pack(1)
#endif
/* WZCL Indication callback ID */
enum
{
  WZCL_LOSS_COORDINATOR_IND_CBID,
  MAX_WZCL_CB_FUNC               // Must be at the bottom of the list
};

typedef enum
{
  MODE_START    =(uint8)0x00,
  MODE_MINUTE   =(uint8)0x01,
  MODE_HOUR     =(uint8)0x02,
  MODE_DAY      =(uint8)0x03,
  MODE_CMD      =(uint8)0x04,
  MODE_THD      =(uint8)0x05
}DetectMode_t;

typedef enum
{
  STAGE_ZERO    =(uint8)0x00,
  STAGE_ONE     =(uint8)0x01,
  STAGE_TWO     =(uint8)0x02,
  STAGE_THREE   =(uint8)0x03,
}DetectStage_t;

typedef struct
{
  uint8      rssi;             /* The received RF power in units dBm */
} wzclSignal_t;


typedef struct
{
  uint8 HeartCount;
} ChildDevice_t;

typedef struct
{
  uint8             EnableAlarm;
  DetectMode_t      DetectMode;
  DetectStage_t     DetectStage;
  uint32            AlarmDuration;
  uint16            MinThreshold;
  int16             VoltageOffset;
}LowBatteryAlarm_t;

typedef struct
{
  uint8     ctrlChar;
  uint8     cmdIndCh;
  uint8     devId;
  uint8     rsLen;
  uint8     extEpNums;
  uint8     defaultBlinkEnable;
#if !defined(RTR_NWK)
  uint8     maxPollFailureRetries;
#endif
  LowBatteryAlarm_t lowBatteryAlarm;
  uint8     MinutesSendHeartToCoord;
  uint8     MinutesHeartWithApsAck;
  uint32    heartBeatIntval;
} wzclDevInfo_t;

typedef struct
{
  uint8 FWVersion;
  uint8 FWDate[3];
  uint8 License[6];
  uint8 FTDate[3];
  uint8 HWVersion;
  //uint8 HWId[10];
} wzclProductInfo_t;

typedef struct
{
  uint8 FWVersion[2];
  uint8 FWDate[6];
  uint8 License[12];
  uint8 FTDate[6];
  uint8 HWVersion[2];
  //uint8 HWId[10];
} wzclProductInfostr_t;

// WZCL header - frame Header
typedef struct
{
  uint8  frametype;
  uint8  fixS;
  uint8  fixM;
} wzclFrameHdr_t;

// OS Command format  设备上线命令格式
typedef struct
{
  wzclFrameHdr_t fd;
  uint8          deviceType;
  uint8          deviceId[2];
  uint8          shortAddr[4];
  uint8          extaddrs[16];
  uint8          fixEnd[4];
} wzclOSCmd_t;

typedef struct
{
  uint8     logicalType;
  uint8     deviceId;
  uint8     shortAddr[4];
  uint8     extaddrs[16];
  uint8     groupId;
} wzclOSCmdInd_t;

// RS Command format 设备刷新命令格式
typedef struct
{
  wzclFrameHdr_t fd;
  uint8          deviceType;
  uint8          deviceId;
  uint16         shortAddr;
  ZLongAddr_t    extaddr;
  uint8          fixc;
  uint8          *pDevStat;
  uint8          fixEnd[2];
} wzclRSCmd_t;

typedef struct
{
  uint8         shortAddr[4];
  uint8         fixstring[6];
  uint8         deviceType;
  uint8         deviceId;
  uint8         extaddrs[16];
  uint8         fixc;
} wzclRSCmdInd_t;

// DS Command format 设备被控后被动返回数据
typedef struct
{
  wzclFrameHdr_t fd;
  uint8          fixstring1[5];
  uint8          deviceId;
  uint16         shortAddr;
  uint8          fixstring2[4];
  uint8          rssi;
  uint8          lqi;
  uint8          *pDevStat;
  uint8          fixEnd[2];
} wzclDsCmd_t;

typedef struct
{
  uint8          shortAddr[4];
  uint8          fixstring[5];
  uint8          cmdIndCh;
} wzclDsCmdInd_t;

// ES Command format
typedef struct
{
  wzclFrameHdr_t fd;
  uint16         shortAddr;
  uint8          *pDevStat;
  uint8          fixEnd[2];
} wzclESCmd_t;

// HS Command format  设备心跳命令
typedef struct
{
  wzclFrameHdr_t fd;
  uint16         shortAddr;
  uint8          fixEnd[2];
} wzclHSCmd_t;

// HS Command format  设备心跳命令
typedef struct
{
  uint8         shortAddr[4];
} wzclHSCmdInd_t;

// 内部Rsii信号值请求
typedef struct
{
  uint8         rssiCmd;
} wzclInterRssiReq_t;

// 内部Rsii信号值请求
typedef struct
{
  uint8         rssiCmd;
  NLDE_Signal_t sigUp;
} wzclInterRssiRsp_t;

// 内部Rsii信号值响应
typedef struct
{
  uint8         rssiCmd;
  NLDE_Signal_t sigUp;
  NLDE_Signal_t sigDowm;
} wzclRssiInd_t;

// US Command format  设备掉线命令
typedef struct
{
  wzclFrameHdr_t fd;
  uint16         shortAddr;
  uint8          fixEnd[2];
} wzclUSCmd_t;

// AS Command format  设备报警命令
typedef struct
{
  wzclFrameHdr_t fd;
  uint16         shortAddr;
  uint8          alarmCode;
  uint8          alarmed;
  uint8          fixEnd[2];
} wzclAsCmd_t;

// AS Command format  设备低压报警指示
typedef struct
{
  uint8          shortAddr[4];
  uint8          fixstring[5];
  uint8          cmdIndCh;
  uint8          alarmMark;
  uint8          lowBattery;
} wzclAsCmdInd_t;

// AS Command format  设备低压报警指示
typedef struct
{
  uint8          cmdIndCh;
  uint8          alarmMark;
  uint8          alarmState;
} wzclAsMsg_t;

typedef struct /* 12 bytes*/
{
  uint8     rxLen;
  uint8     srcdAddr[2];
  uint8     pktType[3];
  uint8     cmd1;
  uint8     cmd2;
  uint8     cmd;
  uint8     shortAddr[4];
} wzclCmdReqHdr_t;

typedef struct
{
  uint8     shortAddr[4];
  uint8     pktType[3];
  uint8     cmd1;
  uint8     cmd2;
  uint8     cmdtype;
} wzclCmdRspHdr_t;

typedef struct
{
  wzclCmdReqHdr_t  hdr;
  union
  {
  uint8     cmds[4];
  uint8     Data[20];
  }asdu;
  uint8     rssi;
} wzclCmd_t;

// Associated Device List Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8         startRelation;
  uint8         endRelation;
} wzclAssocDevListReq_t;

// Associated Device List Response Command Format
typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            AssocDevCnt;
} wzclAssocDevListRsp_t;

//Rssi Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
} wzclDevRssiReq_t;

typedef struct
{
  wzclCmdReqHdr_t   hdr;
  wzclProductInfostr_t pdInfoStr;
} wzclProductInfoReq_t;
typedef struct
{
  wzclCmdReqHdr_t   hdr;
  uint8             distest;
  uint8             autoJoin;
} wzclDisableTestReq_t;
typedef struct
{
  wzclCmdRspHdr_t   hdr;
  uint8             SpecCmd;
  uint8             distest;
} wzclDisableTestRsp_t;

//Rssi Response Command Format
typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  wzclSignal_t    sigDowm;
  wzclSignal_t    sigUp;
} wzclDevRssiRsp_t;

// Product Infomation Response Command Format
typedef struct
{
  wzclCmdRspHdr_t       hdr;
  uint8                 SpecCmd;
  wzclProductInfo_t     ProductInfo;
}wzclProductInfoRsp_t;

typedef struct
{
  wzclCmdRspHdr_t       hdr;
  uint8                 SpecCmd;
  uint8                 versionString[10];
}wzclRevisionRsp_t;

// Product Infomation Response Command Format
typedef struct
{
  wzclCmdRspHdr_t       hdr;
  uint8                 SpecCmd;
#if defined(DEBUG_MSG_ID)
  uint16                msgId;
#endif
  uint16                msg;
}wzclNwkReporting_t;

// Firmware Infomation Response Command Format
typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            lowBattery;
} wzclBatteryStateRsp_t;
typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            Voltage[2];
} wzclVoltageRsp_t;

typedef struct
{
  wzclCmdReqHdr_t  hdr;
  DetectMode_t     DetectMode;
  uint8            AlarmDuration[3];
} wzclDetectModeReq_t;

typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  DetectMode_t     DetectMode;
  union
  {
    uint8           MinThreshold[2];
    uint32          AlarmDuration;
  }load;
} wzclDetectModeRsp_t;

typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            location[4];
} wzclLocationReq_t;

typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint16           location;
} wzclLocationRsp_t;

// leave network and reset to factory Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            joinRole;
  uint8            leavetime[3];
} wzclLeaveRestoreFactoryReq_t;

// Permit Joining Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;

  uint8            period[3];
} wzclPermitJoiningReq_t;

// Enable OS ack Request commang Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;

  uint8            enable;
} wzclEnableOsAckReq_t;

//Remove device from Assoc Device List
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            nwkChar[4];
} RemoveDevInAssocReq_t;

// Permit Joining Response Command Format
typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            permitTime;
} wzclPermitJoiningRsp_t;

// Blink Device system Led Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            blink[3];
} wzclBlinkLedReq_t;

// Config Poll Rate Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            pollrate[2];
} wzclPollRateConfigReq_t;

// Transmit Power Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            sign;
  uint8            transmitPower[2];
} wzclTransmitPowerReq_t;

typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            sign;
  uint8            transmitPower;
} wzclTransmitPowerRsp_t;
// Delete binding Item Request Command Format
typedef struct
{
  wzclCmdReqHdr_t  hdr;
  uint8            endpoint[3];
} wzclDeleteBindingItemReq_t;

typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            endpoint;
} wzclDeleteBindingItemRsp_t;

typedef struct
{
  uint8     shortAddr[4];
  uint8     cmd3;
  uint8     cmds[4];
  uint8     rssidown;
} wzclCmdInfoInd_t;

typedef struct
{
  wzclCmdRspHdr_t  hdr;
  uint8            SpecCmd;
  uint8            resetCode;
} wzclLeaveInd_t;
#ifdef ewarm
#pragma pack()
#endif

typedef struct
{
  uint8   enableCheckIn;
  uint8   enablelongPoll;
#if defined(LARGER_NETWORK)
  uint8   enableHeartBeatPoll;
#endif
  uint32  checkInInterval;
  uint32  longPollInterval;
  uint16  shortPollInterval;
  uint32  fastPollTimeout;
} wzclPollControl_t;

typedef struct
{
  uint32 count;
  uint8  active;
}wzcl_NwkLossTimes_t;
/*********************************************************************
 * GLOBAL VARIABLES
 */
extern const uint8 FWVersion;
extern const uint8 FWDate[];
extern endPointDesc_t wzcl_epDesc;
extern endPointDesc_t wzcl_epDesc1;
extern endPointDesc_t wzcl_epDesc2;
extern endPointDesc_t wzcl_epDesc3;
extern endPointDesc_t wzcl_epDesc4;
extern endPointDesc_t wzcl_epDesc5;
extern endPointDesc_t wzcl_epDesc6;
extern wzcl_NwkLossTimes_t NwkLossTimes;
#if defined(RTR_NWK)
extern bool CoordinatorLeave;
#endif
extern wzclAsMsg_t BatteryAlarm;
extern wzclDevInfo_t DevInfo;
extern const uint8 License[10];
extern const uint8 DisableManufacture;
/*********************************************************************
* FUNCTIONS
*/
#define WZCL_ProcessBindingMessage(pkt) UserAppProcessBindingMessage(pkt)

#define WZCL_CoordinatorOfflineCallback(a) UserAppCoordinatorOfflineCallback(a)
/*
 * Task Initialization for the WZCL Application
 */
void WZCL_Init(uint8 task_id);
/*
 * Task Event Processor for the WZCL Application
 */
uint16 WZCL_ProcessEvent( uint8 task_id, uint16 events );
/*
 * Send User‘s As Messages
 */
void WZCL_SendAsMessage(wzclAsMsg_t *iMsg);
void WZCL_SendAsWithAckMessage(wzclAsMsg_t *iMsg);
#if defined(LARGER_NETWORK)
void WZCL_ActiveDevice(void);
#endif
/*
 *
 */
bool WZCL_ForbitKeyControlInNetworkJoining(void);
void WZCL_SetKeyControlInNetworkJoining(bool enable);
void WZCL_PermitJoin(uint8 period);
uint8 WZCL_ReadKeyMultiClicks(void);
uint8 WZCL_ReadKeyHoldPress(void);
void WZCL_InterRssiReqMessage(void);
void WZCL_FindAndJoinNetwork(void);
void WZCL_LeaveAndRestoreFactory(void);
void WZCL_CommandLeaveAndRestoreFactoryCallBack(void);
void WZCL_SendRsMessage(void);
/*
 * Send User‘s Ds Messages
 */
void WZCL_SendDsMessage(uint8 * iMsg, uint8 len);
void WZCL_SendDsWithAckMessage(uint8 * iMsg, uint8 len);
void WZCL_SendBindingMessage(endPointDesc_t * srcEP,uint16 cID, uint16 len, uint8 *buf);
void WZCL_SendBindingWithAckMessage(endPointDesc_t * srcEP, uint16 cID, uint16 len, uint8 *buf);
void WZCL_InBindingReqMessage(uint8 endpoint ,cId_t* bindingInClusters,uint8 clusterlists);
void WZCL_OutBindingReqMessage(uint8 endpoint ,cId_t* bindingOutClusters,uint8 clusterlists);
/*
 * Processor Command Call Back for the wzcl Application
 */
void UserAppProcessCommandCallBack(uint8 inMsgLen, uint8 *inMsg,
                                    uint8 *outMsgLen, uint8 *outMsg);

void UserAppProcessReportingCommand(uint8 cmd ,uint8*inMsg);
void UserAppProcessBindingMessage(afIncomingMSGPacket_t *pkt);
void UserAppCoordinatorOfflineCallback(bool offline);
void UserAppJoinedNetworkCallBack(void);
void UserAppSetNormalPollRateCallBack(uint16* pollrate);
void UserAppPreSendRsMessageCallBack( uint8* rsMsg );
void UserAppSetDevInfo(wzclDevInfo_t * devInfo);
void UserAppSetPollRate(wzclPollControl_t *pollrate);
void UserAppStartNwkJoiningCallBack(void);
uint16 UserAppPowerOnJoinNwkDelayCallBack(void);
void UserAppSendHeartBeatCallBack(void);
void UserAppRssiBlinkLed(wzclDevRssiRsp_t *pRsp);
void UserAppExtendLightBlink(uint8 numblinks);
ZStatus_t UserAppRegisterForWzclCB( uint8 indID, pfnWzclCb pFn );
ZStatus_t UserAppDeregisterForWzclCB( uint8 indID );
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* WZCL_H */
