/**************************************************************************************************
  Filename:       DeviceConfig.h
  Revised:        $Date: 2016-02-26 $
  Revision:       $Revision: 0002 $

  Description:    This file contains the Device Configuration definitions.
**************************************************************************************************/

#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

/*********************************************************************
 * INCLUDES
 */

/* Gateway */
#if defined(COORDINAROE_GATEWAY)
#include "GatewayUserApp.h"
#endif

/* SampleDevice */
#if defined(SAMPLE_DEVICE)
#include "SampleDeviceUserApp.h"
#endif

/*SW16ZeroFirewireSwitch*/
#if defined(SW16_ZERO_FIREWIRE_SWITCH)
#include "SW16ZeroFirewireSwitchUserApp.h"
#endif

/*Wall Socket Switch*/
#if defined(WALL_SOCKET_SWITCH)
#include "WallSocketSwitchUserApp.h"
#endif

/* Smoking detector honeywell */
#if defined(SMOKING_DETECTOR_HONEYWELL)
#include "Smoking_Deteor_HoneyWellUserApp.h"
#endif

/* Five Window Device */
#if defined(FIVE_WINDOW)
#include "WindowDeviceUserApp.h"
#endif


/* RF Signal Detector*/
#if defined(RF_SIGNAL_DETECTOR)
#include "RFSignalDetectorUserApp.h"
#endif

/* Scene Switch */
#if defined(SCENE_SWITCH)
#include "SceneSwitchUserApp.h"
#endif
/* Alarm */
#if defined(ALARM)
#include "AlarmUserApp.h"
#endif

/* VOC Elock */
#if defined(VOC_ELOCK)
#include "VocElockUserApp.h"
#endif

/* Door Bell */
#if defined(DOOR_BELL)
#include "DoorBellUserApp.h"
#endif

/* Door Bell Button */
#if defined(DOOR_BELL_BUTTON)
#include "DoorBellButtonUserApp.h"
#endif

/* Flood Senor */
#if defined(FLOOD_SENOR)
#include "FloodSenorUserApp.h"
#endif

/* Inside Switch*/
#if defined(INSIDE_SWITCH)
#include "InsideSwitchUserApp.h"
#endif

/* PMDust Monitor */
#if defined(PMDUST_MONITOR)
#include "PMDustMonitorUserApp.h"
#endif

/*  Dust Monitor */
#if defined(DUST_MONITOR)
#include "DustMonitorUserApp.h"
#endif

/* Color Dimmable Light */
#if defined(COLOR_DIMMABLE_LIGHT)
#include "ColorDimmableLightUserApp.h"
#endif

/* Ceiling Mounted Luminaire */
#if defined(CEILINGMOUNTEDLUMINAIRE_DEVICE)
#include "CeilingMountedLuminaireUserApp.h"
#include "FLASH.h"
#include "PWM.h"
#endif

/* Light Modulation Lamp */
#if defined(LIGHT_MODULATION_LAMP)
#include "LightModulationLampUserApp.h"
#include "PWM.h"
#endif

/*  Dimmer Switch */
#if defined(DIMMER_SWITCH)
#include "DimmerSwitchUserApp.h"
#endif

/* Mobile Socket */
#if defined(MOBILE_SOCKET)
#include "MobileSocketUserApp.h"
#endif

/* Mobile Metering Socket */
#if defined(MOBILE_METERING_SOCKET)
#include "MobileMeteringSocketUserApp.h"
#endif

/* Infrared Repeater */
#if defined(INTRARED_REPEATEER)
#include "InfraredRepeaterUserApp.h"
#endif

/* Door Magnetic Detector */
#if defined(DOOR_MAGNETIC_DETECTOR)
#include "DoorMagneticDetectorUserApp.h"
#endif

/* Infrared Intrusion Detector */
#if defined(INFRARED_INTRUSION_DETECTOR)
#include "InfraredIntrusionDetectorUserApp.h"
#endif

/* Temperature Humidity Sensor */
#if defined(TEMPERATURE_HUMIDITY_SENSOR)
#include "TemperatureHumiditySensorUserApp.h"
#endif

/* Combustible Gas Detector */
#if defined(COMBUSTIBLE_GAS_DETECTOR)
#include "CombustibleGasDetectorUserApp.h"
#endif

/*Smoke  Detector*/
#if defined(SMOKE_DETECTOR)
#include "SmokeDetectorUserApp.h"
#endif

/* 按键式窗帘控制器 */
#if defined(CURTAIN_CONTROL_KEY)
#include "CurtainControlKeyUserApp.h"
#endif

/* 触摸窗帘控制器 */
#if defined(CURTAIN_CONTROL_TOUCH)
#include "CurtainControlTouchUserApp.h"
#endif

/* 无线中继器 */
#if defined(REPEATER_CC2530)
#include "RepeaterCC2530UserApp.h"
#endif

#if defined(WIRED_WIRELESS_CONVERTOR)
#include "WiredWirelessConverterUserApp.h"
#endif

/* 单火线开关01/02/03/04型  */
#if defined(SINGLE_FIREWIRE_01M) || defined(SINGLE_FIREWIRE_02M) || defined(SINGLE_FIREWIRE_03M) || defined(SINGLE_FIREWIRE_04M)
#include "SingleFirewireSwitchUserApp.h"
#endif

/* 单火线触摸开关01/02/03/04型 */
#if defined(SINGLE_FIREWIRE_TOUCH_01M) || defined(SINGLE_FIREWIRE_TOUCH_02M) || defined(SINGLE_FIREWIRE_TOUCH_03M) || defined(SINGLE_FIREWIRE_TOUCH_04M)
#include "SingleFirewireTouchSwitchUserApp.h"
#endif

/* 零火线开关01/02/03型 */
#if defined(ZERO_FIREWIRE_01M) || defined(ZERO_FIREWIRE_02M) ||defined(ZERO_FIREWIRE_03M)
#include "ZeroFirewireSwitchUserApp.h"
#endif

/* 零火线触摸开关01/02/03/04型 */
#if defined(ZERO_FIREWIRE_TOUCH_01M) || defined(ZERO_FIREWIRE_TOUCH_02M) ||defined(ZERO_FIREWIRE_TOUCH_03M) || defined(ZERO_FIREWIRE_TOUCH_04M)
#include "ZeroFirewireTouchSwitchUserApp.h"
#endif

/* 零火线绑定开关01/02/03型 */
#if defined(ZERO_FIREWIRE_BINDING_01M) || defined(ZERO_FIREWIRE_BINDING_02M) ||defined(ZERO_FIREWIRE_BINDING_03M)
#include "ZeroFirewireBindingSwitchUserApp.h"
#endif

/* 零火线触摸绑定开关01/02/03型 */
#if defined(ZERO_FIREWIRE_TOUCH_BINDING_01M) || defined(ZERO_FIREWIRE_TOUCH_BINDING_02M) ||defined(ZERO_FIREWIRE_TOUCH_BINDING_03M) || defined(ZERO_FIREWIRE_TOUCH_BINDING_04M)
#include "ZeroFirewireTouchBindingSwitchUserApp.h"
#endif

/* 物联30A一位计量开关*/
#if defined(METERING_SWITCH)
#include "MeteringSwitchUserApp.h"
#endif

#if defined(SMART_METERING_SWITCH)
#include "SmartMeteringSwitchUserApp.h"
#endif
#if defined(ELOCK_JIAWEI)
#include "ELOCK_JIAWEIUserApp.h"
#endif
#if defined(HAIERAIRQ)
#include "HaierAirQUserApp.h"
#endif

#if defined(HAIERFAUCET)
#include "HaierFaucetUserApp.h"
#endif

#if defined(HAIERFAUCET)
#include "HaierFaucetUserApp.h"
#endif

#if defined(AOKMOTOR)
#include "AokMotorUserApp.h"
#endif

#if defined(HOSJOY)
#include "HosJoyDeviceUserApp.h"
#endif
#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * VARIABLES
 */
/*
 * the User Application task Id
 */
extern uint8 UserApp_TaskID;
/*********************************************************************
* GLOBAL FUNCTION
*/
/*
 * Task Initialization for the User Application
 */
extern void UserApp_Init( uint8 task_id );
/*
 * Task Event Processor for the User Application
 */
extern uint16 UserApp_ProcessEvent( uint8 task_id, uint16 events );

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_CONFIG_H */
