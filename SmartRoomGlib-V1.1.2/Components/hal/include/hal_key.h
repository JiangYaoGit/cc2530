/**************************************************************************************************
  Filename:       hal_key.h
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the KEY Service.


  Copyright 2005-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ìAS ISî WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef HAL_KEY_H
#define HAL_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *                                             INCLUDES
 **************************************************************************************************/
#include "hal_board.h"

/**************************************************************************************************
 * MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/

/* Interrupt option - Enable or disable */
#define HAL_KEY_INTERRUPT_DISABLE    0x00
#define HAL_KEY_INTERRUPT_ENABLE     0x01

#define HAL_KEY_CLICKED_VALUE        600    //600ms

#ifdef HOLD_AUTO_START
#define HAL_KEY_HOLD_VALUE           9400   //9.4s
#else
#define HAL_KEY_HOLD_VALUE           5400   //5.4s
#endif

/* Key state - shift or nornal */
#define HAL_KEY_STATE_NORMAL          0x00
#define HAL_KEY_STATE_SHIFT           0x01

#define HAL_KEY_STATE_SHIFT1           0x02
#define HAL_KEY_STATE_SHIFT2           0x03
#define HAL_KEY_STATE_SHIFT3           0x04

/* Buttons */
#define HAL_KEY_SW_1 0x01  // Button S1 if available
#define HAL_KEY_SW_2 0x02  // Button S2 if available
#define HAL_KEY_SW_3 0x04  // Button S3 if available
#define HAL_KEY_SW_4 0x08  // Button S4 if available
#define HAL_KEY_SW_5 0x10  // Button S5 if available
#define HAL_KEY_SW_6 0x20  // Button S6 if available
#define HAL_KEY_SW_7 0x40  // Button S7 if available
#define HAL_KEY_SW_8 0x80  // Button S8 if available
#define HAL_KEY_SW_9 0x100  // Button S9 if available

#if defined(DIMMABLE_SWITCH_HAS) || defined(SCENE_SWITCH_HAS) || defined(ADAPTIVE_SWITCH_KEY_HAS) || defined(MULTIFUNCTIONL_SWITCH_01M)
#define HAL_KEY_RISING_EDGE   0
#define HAL_KEY_FALLING_EDGE  1

#define SYS_KEY_1 0x01
#define SYS_KEY_2 0x02

#define SCENE_KEY_1 0x03
#define SCENE_KEY_2 0x04
#define SCENE_KEY_3 0x05
#define SCENE_KEY_4 0x06
#define SCENE_KEY_5 0x07
#define SCENE_KEY_6 0x08

#define DIMMER_KEY_1 0x09
#define DIMMER_KEY_2 0x0A
#define DIMMER_KEY_3 0x0B
#define DIMMER_KEY_4 0x0C
#endif
/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halKeyCBack_t) (uint16 keys, uint8 state);


typedef struct
{
  uint8 LongHoldActive;                 //≥§∞¥ºÏ≤‚¥•∑¢
  uint8 MultiClickCount;                //∂‡∞¥º∆ ˝£¨∂‡∞¥Ω· ¯∫Û«Â¡„
  uint8 MultiClicks;                    //∂‡∞¥º∆ ˝±£¥Ê
  uint8 HoldPress;                      //≥§∞¥»∑»œ±Í÷æ
  uint16 ClickValue;                    //∂Ã∞¥ ±º‰∑ß÷µ
  uint16 HoldValue;                     //≥§∞¥ ±º‰∑ß÷µ
  uint8 Shift;                          //Õ¨ ±∞¥œ¬Setº¸∫Õ¥À∞¥º¸±Í÷æ

  uint8 RasingInteruptSetFlag;          //∞¥º¸≥§∞¥ ∂±∫Û£¨…Ë÷√¡À…œ…˝—ÿ¥•∑¢
  uint8 RasingInteruptTriggerFirstFlag; //∞¥º¸≥§∞¥ ∂±∫Û£¨…Ë÷√¡À…œ…˝—ÿ¥•∑¢∫Û£¨¥•∑¢¡À°∞µ⁄“ª¥Œ°±…œ…˝—ÿ÷–∂œ±Í÷æ
}UserKeyMultiClickAndHoldPressDetect_t;


/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 **************************************************************************************************/
extern bool Hal_KeyIntEnable;
extern halKeyCBack_t pHalKeyProcessFunction;
//Define for system set key
extern uint32 HalKeyHoldValue;
extern uint16 HalKeyCurrentKeys;          /* used to store current key state in interrupt mode */
extern uint8 HalKeyMultiClickCount;
extern uint8 HalKeyLongHoldActive;
extern uint8 KeyMultiClicks;
extern uint8 KeyHoldPress;
#if defined(MULTIFUNCTIONL_SWITCH_01M)

extern UserKeyMultiClickAndHoldPressDetect_t SysKey1Detect;

extern UserKeyMultiClickAndHoldPressDetect_t SysKey2Detect;

#if defined(S1KEY) || defined(S2KEY) ||defined(S3KEY) || defined(N1KEY) || defined(N2KEY) || defined(N3KEY)
//«Èæ∞º¸1∂Ã∞¥°¢≥§∞¥ºÏ≤‚œ‡πÿ
extern UserKeyMultiClickAndHoldPressDetect_t SceneKey1Detect;
#endif
#if defined(S2KEY) || defined(S3KEY) || defined(N2KEY) || defined(N3KEY)
//«Èæ∞º¸2∂Ã∞¥°¢≥§∞¥ºÏ≤‚œ‡πÿ
extern UserKeyMultiClickAndHoldPressDetect_t SceneKey2Detect;
#endif
#if defined(S3KEY) || defined(N3KEY)
//«Èæ∞º¸3∂Ã∞¥°¢≥§∞¥ºÏ≤‚œ‡πÿ
extern UserKeyMultiClickAndHoldPressDetect_t SceneKey3Detect;
#endif
#endif
/**************************************************************************************************
 *                                             FUNCTIONS - API
 **************************************************************************************************/

/*
 * Initialize the Key Service
 */
extern void HalKeyInit( void );

/*
 * Configure the Key Service
 */
extern void HalKeyConfig( bool interruptEnable, const halKeyCBack_t cback);

/*
 * Read the Key status
 */
extern uint16 HalKeyRead( void);

/*
 * Enter sleep mode, store important values
 */
extern void HalKeyEnterSleep ( void );

/*
 * Exit sleep mode, retore values
 */
extern uint8 HalKeyExitSleep ( void );

/*
 * This is for internal used by hal_driver
 */
extern void HalKeyPoll ( void );

/*
 * This is for internal used by hal_sleep
 */
extern bool HalKeyPressed( void );

extern uint8 hal_key_keys(void);

extern uint8 hal_key_int_keys(void);
/*
 * This is for check key long press
 */
extern void HalKeyHoldCheck(void);
/*
 * This is for check key multi clicks
 */
extern void HalKeyMultiClickCheck(void);
/*
 * This is only for check key multi clicks
 */
extern void HalKeyOnlyMultiClickCheck(void);
/*
 * This is for Process Pin interrupt Enable for  Switch
 */
extern void HalKeyInterruptProcess1(void);
extern void HalKeyInterruptProcess2(void);
extern void HalKeyInterruptProcess3(void);
extern void HalKeyInterruptProcess4(void);

#if defined(MULTIFUNCTIONL_SWITCH_01M)
void SetSceneKeyInterruptRisingOrFallingEdge (uint8 Key, uint8 Edge );
#endif
/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
