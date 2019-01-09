#ifndef LOWBATTERY_MOTOR_SERVER_H_
#define LOWBATTERY_MOTOR_SERVER_H_

#include "hal_types.h"
#include "AOKMotorUart.h"
#include "OSAL.h"
#include "DeviceConfig.h"
#include "hal_led.h"
/*********************************************************************/
typedef struct{
  uint8 Position;	
  bool Dir;            // ���ת������            
  uint8 ShouLa;
  uint8 Status;	
  uint8 Travel;
  uint8 Rece_flag;
  uint8 Uart_ASk_Flag;
  uint8 CRC_buff[2];
  uint8 chCRCHi;
  uint8 chCRCLo;
  uint8 wIndex;
}DUYAMOTOR;

extern uint8 CommndFlag;

extern uint8 UART_SEND_DOWN[10];    //ģ���յ���λ�����ݺ󣬸��豸���͵�����
extern uint8 LEN_UART_SEND_DOWN;   //�������ݵĳ���
extern DUYAMOTOR Motor;
extern uint8 dataReportUp[2];

extern void Delay_device_1u(uint32 microSecs);
extern void GetMotorStatus_Event(void);
extern void SetMotorDir(void);
extern void DeletMotorStroke(void);

extern void LeaveNetResponse(void);    
extern void JoinNetReponseoK(void);
extern void QueryMotorStroke(void);

extern void CRC16(uint8 *puchMsg,uint8 DataLen);
#endif