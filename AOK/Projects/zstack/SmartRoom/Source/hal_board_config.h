/*******************************************************************************
  Filename:       hal_board_config.h
  Revised:        $Date: 2016-02-26 $
  Revision:       $Revision: 0007 $

  Description:    This file contains the Device Board Configuration definitions.
*******************************************************************************/

#ifndef HAL_BOARD_CONFIG_H
#define HAL_BOARD_CONFIG_H

/*********************************************************************
 * INCLUDES
 */

/* Gateway */
#if defined(COORDINAROE_GATEWAY)
#include "GatewayBoardCfg.h"
#endif

/* Sample Device */
#if defined(SAMPLE_DEVICE)
#include "SampleDeviceBoardCfg.h"
#endif

/*SW16ZeroFirewireSwitch*/
#if defined(SW16_ZERO_FIREWIRE_SWITCH)
#include "SW16ZeroFirewireSwitchBoardCfg.h"
#endif

/*Wall Socket Switch*/
#if defined(WALL_SOCKET_SWITCH)
#include "WallSocketSwitchBoardCfg.h"
#endif

/* Smoking detector honeywell */
#if defined(SMOKING_DETECTOR_HONEYWELL)
#include "Smoking_Deteor_HoneyWellBoardCfg.h"
#endif


#if defined(FIVE_WINDOW)
#include "WindowDeviceBoardCfg.h"
#endif

/* RF Signal Detector*/
#if defined(RF_SIGNAL_DETECTOR)
#include "RFSignalDetectorBoardCfg.h"
#endif

/* Scene Switch  */
#if defined(SCENE_SWITCH)
#include "SceneSwitchBoardCfg.h"
#endif

/* Door Bell */
#if defined(DOOR_BELL)
#include "DoorBellBoardCfg.h"
#endif

/* Alarm */
#if defined(ALARM)
#include "AlarmBoardCfg.h"
#endif

/* VOC Elock */
#if defined(VOC_ELOCK)
#include "VocElockBoardCfg.h"
#endif

/* Door Bell Button */
#if defined(DOOR_BELL_BUTTON)
#include "DoorBellButtonBoardCfg.h"
#endif

/* Flood Senor */
#if defined(FLOOD_SENOR)
#include "FloodSenorBoardCfg.h"
#endif

/* Inside Switch*/
#if defined(INSIDE_SWITCH)
#include "InsideSwitchBoardCfg.h"
#endif

/* PMDust Monitor */
#if defined(PMDUST_MONITOR)
#include "PMDustMonitorBoardCfg.h"
#endif

/* Dust Monitor */
#if defined(DUST_MONITOR)
#include "DustMonitorBoardCfg.h"
#endif

/* Color Dimmable Light */
#if defined(COLOR_DIMMABLE_LIGHT)
#include "ColorDimmableLightBoardCfg.h"
#endif

/* Ceiling Mounted Luminaire */
#if defined(CEILINGMOUNTEDLUMINAIRE_DEVICE)
#include "CeilingMountedLuminaireBoardCfg.h"
#endif

/* Light Modulation Lamp */
#if defined(LIGHT_MODULATION_LAMP)
#include "LightModulationLampBoardCfg.h"
#endif

/*  Dimmable Switch */
#if defined(DIMMER_SWITCH)
#include "DimmerSwitchBoardCfg.h"
#endif

/* Mobile Socket */
#if defined(MOBILE_SOCKET)
#include "MobileSocketBoardCfg.h"
#endif

/* Mobile Metering Socket */
#if defined(MOBILE_METERING_SOCKET)
#include "MobileMeteringSocketBoardCfg.h"
#endif

/* Infrared Repeater */
#if defined(INTRARED_REPEATEER)
#include "InfraredRepeaterBoardCfg.h"
#endif

/* Door Magnetic Detector */
#if defined(DOOR_MAGNETIC_DETECTOR)
#include "DoorMagneticDetectorBoardCfg.h"
#endif

/* Infrared Intrusion Detector */
#if defined(INFRARED_INTRUSION_DETECTOR)
#include "InfraredIntrusionDetectorBoardCfg.h"
#endif

/* Temperature Humidity Sensor */
#if defined(TEMPERATURE_HUMIDITY_SENSOR)
#include "TemperatureHumiditySensorBoardCfg.h"
#endif

/* Combustible Gas Detector */
#if defined(COMBUSTIBLE_GAS_DETECTOR)
#include "CombustibleGasDetectorBoardCfg.h"
#endif

/*Smoke  Detector*/
#if defined(SMOKE_DETECTOR)
#include "SmokeDetectorBoardCfg.h"
#endif

/* 按键式窗帘控制器 */
#if defined(CURTAIN_CONTROL_KEY)
#include "CurtainControlKeyBoardCfg.h"
#endif

/* 触摸窗帘控制器 */
#if defined(CURTAIN_CONTROL_TOUCH)
#include "CurtainControlTouchBoardCfg.h"
#endif

/* 无线中继器 */
#if defined(REPEATER_CC2530)
#include "RepeaterCC2530BoardCfg.h"
#endif

#if defined(WIRED_WIRELESS_CONVERTOR)
#include "WiredWirelessConverterBoardCfg.h"
#endif

/* 单火线开关01/02/03/04型  */
#if defined(SINGLE_FIREWIRE_01M) || defined(SINGLE_FIREWIRE_02M) || defined(SINGLE_FIREWIRE_03M) || defined(SINGLE_FIREWIRE_04M)
#include "SingleFirewireSwitchBoardCfg.h"
#endif

/* 单火线触摸开关01/02/03/04型 */
#if defined(SINGLE_FIREWIRE_TOUCH_01M) || defined(SINGLE_FIREWIRE_TOUCH_02M) || defined(SINGLE_FIREWIRE_TOUCH_03M) || defined(SINGLE_FIREWIRE_TOUCH_04M)
#include "SingleFirewireTouchSwitchBoardCfg.h"
#endif

/* 零火线开关01/02/03型 */
#if defined(ZERO_FIREWIRE_01M) || defined(ZERO_FIREWIRE_02M) ||defined(ZERO_FIREWIRE_03M)
#include "ZeroFirewireSwitchBoardCfg.h"
#endif

/* 零火线触摸开关01/02/03/04型*/
#if defined(ZERO_FIREWIRE_TOUCH_01M) || defined(ZERO_FIREWIRE_TOUCH_02M) ||defined(ZERO_FIREWIRE_TOUCH_03M) || defined(ZERO_FIREWIRE_TOUCH_04M)
#include "ZeroFirewireTouchSwitchBoardCfg.h"
#endif

/* 零火线绑定开关01/02/03型 */
#if defined(ZERO_FIREWIRE_BINDING_01M) || defined(ZERO_FIREWIRE_BINDING_02M) ||defined(ZERO_FIREWIRE_BINDING_03M)
#include "ZeroFirewireBindingSwitchBoardCfg.h"
#endif

/* 零火线触摸绑定开关01/02/03型 */
#if defined(ZERO_FIREWIRE_TOUCH_BINDING_01M) || defined(ZERO_FIREWIRE_TOUCH_BINDING_02M) ||defined(ZERO_FIREWIRE_TOUCH_BINDING_03M)||defined(ZERO_FIREWIRE_TOUCH_BINDING_04M)
#include "ZeroFirewireTouchBindingSwitchBoardCfg.h"
#endif

/* 物联30A一位计量开关*/
#if defined(METERING_SWITCH)
#include "MeteringSwitchBoardCfg.h"
#endif

#if defined(SMART_METERING_SWITCH)
#include "SmartMeteringSwitchBoardCfg.h"
#endif
#if defined(ELOCK_JIAWEI)
#include "ELOCK_JIAWEIBoardCfg.h"
#endif

#if defined(HAIERAIRQ)
#include "HaierAirQBoardCfg.h"
#endif

#if defined(HAIERFAUCET)
#include "HaierFaucetBoardCfg.h"
#endif

#if defined(AOKMOTOR)
#include "AOKMotorBoardCfg.h"
#endif

#if defined(HOSJOY)
#include "HosJoyDeviceBoardCfg.h"
#endif


#ifdef __cplusplus
extern "C"
{
#endif
/* ------------------------------------------------------------------------------------------------
 *                                    Push Button Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint8 const HAL_NUM_LEDS;
/*********************************************************************
 * GLOBAL FUNCTION
 */
void HAL_TURN_OFF_LED1(void);
void HAL_TURN_OFF_LED2(void);
void HAL_TURN_OFF_LED3(void);
void HAL_TURN_OFF_LED4(void);
void HAL_TURN_OFF_LED5(void);
void HAL_TURN_OFF_LED6(void);
void HAL_TURN_OFF_LED7(void);
void HAL_TURN_OFF_LED8(void);

void HAL_TURN_ON_LED1(void);
void HAL_TURN_ON_LED2(void);
void HAL_TURN_ON_LED3(void);
void HAL_TURN_ON_LED4(void);
void HAL_TURN_ON_LED5(void);
void HAL_TURN_ON_LED6(void);
void HAL_TURN_ON_LED7(void);
void HAL_TURN_ON_LED8(void);

void HAL_TOGGLE_LED1(void);
void HAL_TOGGLE_LED2(void);
void HAL_TOGGLE_LED3(void);
void HAL_TOGGLE_LED4(void);
void HAL_TOGGLE_LED5(void);
void HAL_TOGGLE_LED6(void);
void HAL_TOGGLE_LED7(void);
void HAL_TOGGLE_LED8(void);

uint8 HAL_STATE_LED1(void);
uint8 HAL_STATE_LED2(void);
uint8 HAL_STATE_LED3(void);
uint8 HAL_STATE_LED4(void);
uint8 HAL_STATE_LED5(void);
uint8 HAL_STATE_LED6(void);
uint8 HAL_STATE_LED7(void);
uint8 HAL_STATE_LED8(void);

uint8 HAL_PUSH_BUTTON1(void);
uint8 HAL_PUSH_BUTTON2(void);
uint8 HAL_PUSH_BUTTON3(void);
uint8 HAL_PUSH_BUTTON4(void);
uint8 HAL_PUSH_BUTTON5(void);
uint8 HAL_PUSH_BUTTON6(void);
uint8 HAL_PUSH_BUTTON7(void);
uint8 HAL_PUSH_BUTTON8(void);

void HAL_CLOCK_STABLE(void);
void HAL_CLOCK_CONFIG(void);

void HAL_IO_CONFIG_INIT(void);
void WatchdogClear(void);
void WatchDogEnableCB(void);
#ifndef ZDO_COORDINATOR
uint32 ChannelSelect(void);
#endif
#if defined(WL_R0042E)
void MAC_RX1_TURN_ON(void);
void MAC_RX2_TURN_ON(void);

void MAC_RX1_TURN_OFF(void);
void MAC_RX2_TURN_OFF(void);
#endif
#ifdef __cplusplus
}
#endif

#endif /* HAL_BOARD_CONFIG_H */
