/**
  ******************************************************************************
  * @file    SampleDeviceBoardCfg.h
  * @author  Wireless Protocol Stack Develop-Dept.
  * @version 1.0.2
  * @date    2015/12/11,yuzz
  * @brief  This file contains the Sample Device Board interface definitions.
  *
  * @verbatim
  *
  ******************************************************************************
  * @attention
  *    modified by yuzz   @2016/02/01   E-mail: zhengzheng.yu@wuliangroup.com
  *
  *
  * <h2><center>&copy; COPYRIGHT 2015 WuLianGroup</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SAMPLE_DEVICE_BOARDCFG_H
#define	SAMPLE_DEVICE_BOARDCFG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//Leds' Macros
/* LED1~2 fix used for system led*/
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_LOW

#define LED2_BV           BV(1)
#define LED2_SBIT         P1_1
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_LOW

#define LED3_BV           BV(0)
#define LED3_SBIT         P2_0
#define LED3_DDR          P2DIR
#define LED3_POLARITY     ACTIVE_LOW

#define LED4_BV           BV(4)
#define LED4_SBIT         P2_4
#define LED4_DDR          P2DIR
#define LED4_POLARITY     ACTIVE_LOW

#define LED5_BV           BV(4)
#define LED5_SBIT         P2_4
#define LED5_DDR          P2DIR
#define LED5_POLARITY     ACTIVE_LOW

#define LED6_BV           BV(4)
#define LED6_SBIT         P2_4
#define LED6_DDR          P2DIR
#define LED6_POLARITY     ACTIVE_LOW

#define LED7_BV           BV(4)
#define LED7_SBIT         P2_4
#define LED7_DDR          P2DIR
#define LED7_POLARITY     ACTIVE_LOW

#define LED8_BV           BV(4)
#define LED8_SBIT         P2_4
#define LED8_DDR          P2DIR
#define LED8_POLARITY     ACTIVE_LOW


//Buttons' Macros
#define PUSH1_BV          BV(2)
#define PUSH1_SBIT        P1_2
#define PUSH1_POLARITY    ACTIVE_LOW

#define PUSH2_BV          BV(5)
#define PUSH2_SBIT        P0_5
#define PUSH2_POLARITY    ACTIVE_LOW

#define PUSH3_BV          BV(4)
#define PUSH3_SBIT        P0_4
#define PUSH3_POLARITY    ACTIVE_LOW

#define PUSH4_BV          BV(5)
#define PUSH4_SBIT        P1_5
#define PUSH4_POLARITY    ACTIVE_HIGH

#define PUSH5_BV          BV(0)
#define PUSH5_SBIT        0
#define PUSH5_POLARITY    ACTIVE_HIGH

#define PUSH6_BV          BV(0)
#define PUSH6_SBIT        0
#define PUSH6_POLARITY    ACTIVE_HIGH

#define PUSH7_BV          BV(0)
#define PUSH7_SBIT        0
#define PUSH7_POLARITY    ACTIVE_HIGH

#define PUSH8_BV          BV(0)
#define PUSH8_SBIT        0
#define PUSH8_POLARITY    ACTIVE_HIGH

#define CHANNEL_SELECT_BIT0  P0_0
#define CHANNEL_SELECT_BIT1  P0_1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* SAMPLE_DEVICE_BOARDCFG_H */
/******************* (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL*********/