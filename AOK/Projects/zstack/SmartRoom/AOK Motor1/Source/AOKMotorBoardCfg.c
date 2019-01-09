/**
  ******************************************************************************
  * @file    SampleDeviceBoardCfg.c
  * @author  Wireless Protocol Stack Develop-Dept.
  * @version 1.0.3
  * @date    2015/12/11,yuzz
  * @brief  This file contains the configuration of the board Service.
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
/*******************************************************************************
 *                                 INCLUDES
 ******************************************************************************/
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_board.h"
#include "OnBoard.h"
//#include "AOKMotorBoardCfg.h"
/*******************************************************************************
 * GLOBAL VARIABLES
 */
/* --------------------------LED Configuration--------------------------------*/
uint8 const HAL_NUM_LEDS = 8;

/*******************************************************************************
 * Global FUNCTIONS
 */
/* -------------------------IO Configration------------------------------------*/
void inline HAL_IO_CONFIG_INIT(void)
{
  LED1_DDR |= LED1_BV;
  LED3_DDR |= LED3_BV;
  HAL_TURN_OFF_LED1();
  HAL_TURN_OFF_LED3();
}
/* ----------------------------LEDs------------------------------------------ */
void inline HAL_TURN_OFF_LED1(void){ LED1_SBIT = LED1_POLARITY (0);}
void inline HAL_TURN_OFF_LED2(void){ LED2_SBIT = LED2_POLARITY (0);}
void inline HAL_TURN_OFF_LED3(void){ LED3_SBIT = LED3_POLARITY (0);}
void inline HAL_TURN_OFF_LED4(void){ LED4_SBIT = LED4_POLARITY (0);}
void inline HAL_TURN_OFF_LED5(void){ LED5_SBIT = LED5_POLARITY (0);}
void inline HAL_TURN_OFF_LED6(void){ LED6_SBIT = LED6_POLARITY (0);}
void inline HAL_TURN_OFF_LED7(void){ LED7_SBIT = LED7_POLARITY (0);}
void inline HAL_TURN_OFF_LED8(void){ LED8_SBIT = LED8_POLARITY (0);}

void inline HAL_TURN_ON_LED1(void){ LED1_SBIT = LED1_POLARITY (1); }
void inline HAL_TURN_ON_LED2(void){ LED2_SBIT = LED2_POLARITY (1); }
void inline HAL_TURN_ON_LED3(void){ LED3_SBIT = LED3_POLARITY (1); }
void inline HAL_TURN_ON_LED4(void){ LED4_SBIT = LED4_POLARITY (1); }
void inline HAL_TURN_ON_LED5(void){ LED5_SBIT = LED5_POLARITY (1); }
void inline HAL_TURN_ON_LED6(void){ LED6_SBIT = LED6_POLARITY (1); }
void inline HAL_TURN_ON_LED7(void){ LED7_SBIT = LED7_POLARITY (1); }
void inline HAL_TURN_ON_LED8(void){ LED8_SBIT = LED8_POLARITY (1); }

void inline HAL_TOGGLE_LED1(void){ if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} }
void inline HAL_TOGGLE_LED2(void){ if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} }
void inline HAL_TOGGLE_LED3(void){ if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} }
void inline HAL_TOGGLE_LED4(void){ if (LED4_SBIT) { LED4_SBIT = 0; } else { LED4_SBIT = 1;} }
void inline HAL_TOGGLE_LED5(void){ if (LED5_SBIT) { LED5_SBIT = 0; } else { LED5_SBIT = 1;} }
void inline HAL_TOGGLE_LED6(void){ if (LED6_SBIT) { LED6_SBIT = 0; } else { LED6_SBIT = 1;} }
void inline HAL_TOGGLE_LED7(void){ if (LED7_SBIT) { LED7_SBIT = 0; } else { LED7_SBIT = 1;} }
void inline HAL_TOGGLE_LED8(void){ if (LED8_SBIT) { LED8_SBIT = 0; } else { LED8_SBIT = 1;} }

uint8 inline HAL_STATE_LED1(void){return(LED1_POLARITY (LED1_SBIT));}
uint8 inline HAL_STATE_LED2(void){return(LED2_POLARITY (LED2_SBIT));}
uint8 inline HAL_STATE_LED3(void){return(LED3_POLARITY (LED3_SBIT));}
uint8 inline HAL_STATE_LED4(void){return(LED4_POLARITY (LED4_SBIT));}
uint8 inline HAL_STATE_LED5(void){return(LED5_POLARITY (LED5_SBIT));}
uint8 inline HAL_STATE_LED6(void){return(LED6_POLARITY (LED6_SBIT));}
uint8 inline HAL_STATE_LED7(void){return(LED7_POLARITY (LED7_SBIT));}
uint8 inline HAL_STATE_LED8(void){return(LED8_POLARITY (LED8_SBIT));}

/* -------------------------- Push Buttons ----------------------------------- */
uint8 inline HAL_PUSH_BUTTON1(void){return(PUSH1_POLARITY (PUSH1_SBIT));}
uint8 inline HAL_PUSH_BUTTON2(void){return(PUSH2_POLARITY (PUSH2_SBIT));}
uint8 inline HAL_PUSH_BUTTON3(void){return(PUSH3_POLARITY (PUSH3_SBIT));}
uint8 inline HAL_PUSH_BUTTON4(void){return(PUSH4_POLARITY (PUSH4_SBIT));}
uint8 inline HAL_PUSH_BUTTON5(void){return(PUSH5_POLARITY (PUSH5_SBIT));}
uint8 inline HAL_PUSH_BUTTON6(void){return(PUSH6_POLARITY (PUSH6_SBIT));}
uint8 inline HAL_PUSH_BUTTON7(void){return(PUSH7_POLARITY (PUSH7_SBIT));}
uint8 inline HAL_PUSH_BUTTON8(void){return(PUSH8_POLARITY (PUSH8_SBIT));}

/* --------------------------Clock Speed------------------------------------- */
void inline HAL_CLOCK_STABLE(void){while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); }
void inline HAL_CLOCK_CONFIG(void){CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ);}
void inline WatchdogClear(void){WDCTL = 0xa0; WDCTL = 0x50;}
void inline WatchDogEnableCB(void){/*WatchDogEnable( WDTIMX );*/}
/***************** (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL***********/