/**
  ******************************************************************************
  * @file    SampleDevice_key.c
  * @author  Wireless Protocol Stack Develop-Dept.
  * @version 1.0.3
  * @date    2015/12/11,yuzz
  * @brief  This file contains the interface to the HAL KEY Service.
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
 *                            INCLUDES
 ******************************************************************************/
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_board.h"
#include "hal_drivers.h"
#include "hal_adc.h"
#include "hal_key.h"
#include "osal.h"
#include "ZDApp.h"
#include "DeviceConfig.h"
#include "OSAL_PwrMgr.h"
#include "UserUart.h"
#include "OnBoard.h"
#include "hal_led.h"


#if (defined HAL_KEY) && (HAL_KEY == TRUE)

/*******************************************************************************
 *                           MACROS
 ******************************************************************************/

/*******************************************************************************
 *                          CONSTANTS
 ******************************************************************************/
#define HAL_KEY_RISING_EDGE   0
#define HAL_KEY_FALLING_EDGE  1

#define HAL_KEY_DEBOUNCE_VALUE  3

/* CPU port interrupt */
#define HAL_KEY_CPU_PORT_0_IF      P0IF
#define HAL_KEY_CPU_PORT_1_IF      P1IF
#define HAL_KEY_CPU_PORT_2_IF      P2IF

#define HAL_KEY_SW_PORT0X_IEN      IEN1
#define HAL_KEY_SW_PORT0X_PXIFG    P0IFG
#define HAL_KEY_SW_PORT0X_IENBIT   BV(5)

#define HAL_KEY_SW_PORT1X_IEN      IEN2
#define HAL_KEY_SW_PORT1X_PXIFG    P1IFG
#define HAL_KEY_SW_PORT1X_IENBIT   BV(4)

#define HAL_KEY_SW_PORT2X_IEN      IEN2
#define HAL_KEY_SW_PORT2X_PXIFG    P2IFG
#define HAL_KEY_SW_PORT2X_IENBIT   BV(1)

/* SW_1 is at P1.2 */
#define HAL_KEY_SW_1_PORT     P1
#define HAL_KEY_SW_1_BIT      BV(2)
#define HAL_KEY_SW_1_SEL      P1SEL
#define HAL_KEY_SW_1_DIR      P1DIR
/* edge interrupt */
#define HAL_KEY_SW_1_EDGEBIT  BV(1) /* Port1 intputs 0-3 interrupt edge configration */
#define HAL_KEY_SW_1_EDGE     HAL_KEY_FALLING_EDGE
/* SW_1 interrupts */
#define HAL_KEY_SW_1_IEN      IEN2  /* CPU interrupt mask register */
#define HAL_KEY_SW_1_IENBIT   BV(4) /* Mask bit for all of Port_1 */
#define HAL_KEY_SW_1_ICTL     P1IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_1_ICTLBIT  BV(2) /* P1IEN - P1.2 enable/disable bit */
#define HAL_KEY_SW_1_PXIFG    P1IFG /* Interrupt flag at source */

/* SW_2 is at P0.5 */
#define HAL_KEY_SW_2_PORT     P1
#define HAL_KEY_SW_2_BIT      BV(7)
#define HAL_KEY_SW_2_SEL      P1SEL
#define HAL_KEY_SW_2_DIR      P1DIR
/* edge interrupt */
#define HAL_KEY_SW_2_EDGEBIT  BV(2) /* Port0 intputs 0-7 interrupt edge configration */
#define HAL_KEY_SW_2_EDGE     HAL_KEY_FALLING_EDGE
/* SW_2 interrupts */
#define HAL_KEY_SW_2_IEN      IEN2  /* CPU interrupt mask register */
#define HAL_KEY_SW_2_IENBIT   BV(4) /* Mask bit for all of Port_0 */
#define HAL_KEY_SW_2_ICTL     P1IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_2_ICTLBIT  BV(7) /* P0IEN - P0.5 enable/disable bit */
#define HAL_KEY_SW_2_PXIFG    P1IFG /* Interrupt flag at source */

/* SW_3 is at P1.3 *///------作为通讯前唤醒
#define HAL_KEY_SW_3_PORT     P1
#define HAL_KEY_SW_3_BIT      BV(3)
#define HAL_KEY_SW_3_SEL      P1SEL
#define HAL_KEY_SW_3_DIR      P1DIR
#define HAL_KEY_SW_3_INP      P1INP
/* edge interrupt */
#define HAL_KEY_SW_3_EDGEBIT  BV(1) /* Port0 intputs 0-7 interrupt edge configration */
#define HAL_KEY_SW_3_EDGE     HAL_KEY_FALLING_EDGE
/* SW_3 interrupts */
#define HAL_KEY_SW_3_IEN      IEN2  /* CPU interrupt mask register */
#define HAL_KEY_SW_3_IENBIT   BV(4) /* Mask bit for all of Port_1 */
#define HAL_KEY_SW_3_ICTL     P1IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_3_ICTLBIT  BV(3) /* P1IEN - P1.2 enable/disable bit */
#define HAL_KEY_SW_3_PXIFG    P1IFG /* Interrupt flag at source */

#define KEY_FORBITON_TIME     100
/*******************************************************************************
 *                              TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 *                             GLOBAL VARIABLES
 ******************************************************************************/
static uint16 halKeySavedKeys;     /* used to store previous key state in polling mode */
static halKeyCBack_t pHalKeyProcessFunction;
static uint8 HalKeyConfigured;
bool Hal_KeyIntEnable;            /* interrupt enable/disable flag */
/* Define for system set key */
uint32 HalKeyHoldValue;
uint16 HalKeyCurrentKeys;          /* used to store current key state in interrupt mode */
uint8 HalKeyMultiClickCount;
uint8 HalKeyLongHoldActive;
// define for system set key
uint8 KeyMultiClicks;
uint8 KeyHoldPress;
/*******************************************************************************
 *                           FUNCTIONS - Local
 ******************************************************************************/
void halProcessKeyInterrupt(void);
extern void Delay_device_1u(uint32 microSecs);
extern void UserApp_SendDeviceState(uint16 sta);
/*******************************************************************************
 *                           FUNCTIONS - API
 ******************************************************************************/

/*******************************************************************************
 * @fn      HalKeyInit
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 ******************************************************************************/
void HalKeyInit( void )
{
  /* Initialize previous key to 0 */
  halKeySavedKeys = 0;

  HAL_KEY_SW_1_SEL &= ~(HAL_KEY_SW_1_BIT);    /* Set pin function to GPIO */
  HAL_KEY_SW_1_DIR &= ~(HAL_KEY_SW_1_BIT);    /* Set pin direction to Input */

  HAL_KEY_SW_3_SEL &= ~(HAL_KEY_SW_3_BIT);    /* Set pin function to GPIO */
  HAL_KEY_SW_3_DIR &= ~(HAL_KEY_SW_3_BIT);    /* Set pin direction to Input */
  HAL_KEY_SW_3_INP |=HAL_KEY_SW_3_BIT;
  
  /* Initialize callback function */
  pHalKeyProcessFunction  = NULL;

  /* Start with key is not configured */
  HalKeyConfigured = FALSE;

  HalKeyMultiClickCount = 0;

  HalKeyLongHoldActive = 0;

  HalKeyHoldValue = HAL_KEY_HOLD_VALUE;
}
/*******************************************************************************
 * @fn      HalKeyConfig
 *
 * @brief   Configure the Key serivce
 *
 * @param   interruptEnable - TRUE/FALSE, enable/disable interrupt
 *          cback - pointer to the CallBack function
 *
 * @return  None
 ******************************************************************************/
void HalKeyConfig (bool interruptEnable, halKeyCBack_t cback)
{
  /* Enable/Disable Interrupt or */
  Hal_KeyIntEnable = interruptEnable;

  /* Register the callback fucntion */
  pHalKeyProcessFunction = cback;

  /* Determine if interrupt is enable or not */
  if (Hal_KeyIntEnable)
  {
    /* Clear the Port interrupt flags */
    HAL_KEY_CPU_PORT_0_IF = 0;
    HAL_KEY_CPU_PORT_1_IF = 0;
    HAL_KEY_CPU_PORT_2_IF = 0;

    /* Rising/Falling edge configuratinn */
    PICTL &= ~(HAL_KEY_SW_1_EDGEBIT);    /* Clear the edge bit */
    /* For falling edge, the bit must be set. */
#if (HAL_KEY_SW_1_EDGE == HAL_KEY_FALLING_EDGE)
    PICTL |= HAL_KEY_SW_1_EDGEBIT;
#endif
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY_SW_1_ICTL |= HAL_KEY_SW_1_ICTLBIT;
    HAL_KEY_SW_1_IEN |= HAL_KEY_SW_1_IENBIT;
    HAL_KEY_SW_1_PXIFG &= ~(HAL_KEY_SW_1_BIT);


    /* Rising/Falling edge configuratinn */
    PICTL &= ~(HAL_KEY_SW_3_EDGEBIT);    /* Clear the edge bit */
    /* For falling edge, the bit must be set. */
#if (HAL_KEY_SW_3_EDGE == HAL_KEY_FALLING_EDGE)
    PICTL |= HAL_KEY_SW_3_EDGEBIT;
#endif
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY_SW_3_ICTL |= HAL_KEY_SW_3_ICTLBIT;
    HAL_KEY_SW_3_IEN |= HAL_KEY_SW_3_IENBIT;
    HAL_KEY_SW_3_PXIFG &= ~(HAL_KEY_SW_3_BIT);

    /* Do this only after the hal_key is configured - to work with sleep stuff */
    if (HalKeyConfigured == TRUE)
    {
      osal_stop_timerEx(Hal_TaskID, HAL_KEY_EVENT);  /* Cancel polling if active */
    }
  }
  else    /* Interrupts NOT enabled */
  {
    HAL_KEY_SW_1_ICTL &= ~(HAL_KEY_SW_1_ICTLBIT); /* don't generate interrupt */
    HAL_KEY_SW_1_IEN &= ~(HAL_KEY_SW_1_IENBIT);   /* Clear interrupt enable bit */

    HAL_KEY_SW_2_ICTL &= ~(HAL_KEY_SW_2_ICTLBIT); /* don't generate interrupt */
    HAL_KEY_SW_2_IEN &= ~(HAL_KEY_SW_2_IENBIT);   /* Clear interrupt enable bit */

    osal_set_event(Hal_TaskID, HAL_KEY_EVENT);
  }

  /* Key now is configured */
  HalKeyConfigured = TRUE;
}
/*******************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   Read the current value of a key
 *
 * @param   None
 *
 * @return  keys - current keys status
 ******************************************************************************/
uint16 HalKeyRead ( void )
{
  uint16 keys = 0;

  if (HAL_PUSH_BUTTON1())
  {
    keys |= HAL_KEY_SW_1;
  }
  else
  {
    keys &= ~HAL_KEY_SW_1;
  }
  if (HAL_PUSH_BUTTON2())
  {
    keys |= HAL_KEY_SW_2;
  }
  else
  {
    keys &= ~HAL_KEY_SW_2;
  }
  
    if (HAL_PUSH_BUTTON3())
  {
    keys |= HAL_KEY_SW_3;
  }
  else
  {
    keys &= ~HAL_KEY_SW_3;
  }
  return keys;
}
/*******************************************************************************
 * @fn      HalKeyPoll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 ******************************************************************************/
void HalKeyPoll (void)
{
  uint16 keys = 0;
  uint8 shift = HAL_KEY_STATE_NORMAL;
  /* If interrupts are not enabled, previous key status and current key status
  * are compared to find out if a key has changed status.
  */
  if (!Hal_KeyIntEnable)
  {
    if (keys == halKeySavedKeys)
    {
      /* Exit - since no keys have changed */
      return;
    }
    /* Store the current keys for comparation next time */
    halKeySavedKeys = keys;
  }
  else
  {
    /* Key interrupt handled here */
    if((HalKeyCurrentKeys == HAL_KEY_SW_1)&&HAL_PUSH_BUTTON1())
    {
      if(HalKeyLongHoldActive)
      {
        HalKeyLongHoldActive = 0;
        HalKeyMultiClickCount = 0;
        osal_stop_timerEx (Hal_TaskID, HAL_KEY_HOLD_EVENT);
      }
      HalKeyMultiClickCount++;
      osal_stop_timerEx (Hal_TaskID, HAL_KEY_MULTI_CLICK_EVENT);
      osal_start_timerEx (Hal_TaskID, HAL_KEY_MULTI_CLICK_EVENT, HAL_KEY_CLICKED_VALUE);
    }

    else if((HalKeyCurrentKeys ==HAL_KEY_SW_3)&& HAL_PUSH_BUTTON3())
    {
      osal_start_timerEx(UserApp_TaskID,OVERTIME_EVT,5000);
    }
  }
  /* Invoke Callback if new keys were depressed */
  if (keys && (pHalKeyProcessFunction))
  {
    (pHalKeyProcessFunction) (keys, shift);
  }
}
/*******************************************************************************
 * @fn      halProcessKeyInterrupt
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 ******************************************************************************/
void halProcessKeyInterrupt (void)
{
  bool valid=FALSE;

  if (HAL_KEY_SW_1_PXIFG & HAL_KEY_SW_1_BIT)  /* Interrupt Flag has been set */
  {
    HAL_KEY_SW_1_PXIFG &= ~(HAL_KEY_SW_1_BIT); /* Clear Interrupt Flag */

    valid = TRUE;
  }
  if (HAL_KEY_SW_3_PXIFG & HAL_KEY_SW_3_BIT)  /* Interrupt Flag has been set */
  {
    HAL_KEY_SW_3_PXIFG &= ~(HAL_KEY_SW_3_BIT); /* Clear Interrupt Flag */

    valid = TRUE;
  }
  if (valid)
  {
    osal_start_timerEx (Hal_TaskID, HAL_KEY_EVENT, HAL_KEY_DEBOUNCE_VALUE);
  }
}
/*******************************************************************************
 * @fn      HalKeyEnterSleep
 *
 * @brief  - Get called to enter sleep mode
 *
 * @param
 *
 * @return
 ******************************************************************************/
void HalKeyEnterSleep ( void )
{
}
/*******************************************************************************
 * @fn      HalKeyExitSleep
 *
 * @brief   - Get called when sleep is over
 *
 * @param
 *
 * @return  - return saved keys
 ******************************************************************************/
uint8 HalKeyExitSleep ( void )
{
  /* Wake up and read keys */
  return ( HalKeyRead () );
}
/*******************************************************************************
 *                              INTERRUPT SERVICE ROUTINE
 ******************************************************************************/

/*******************************************************************************
 * @fn      halKeyPort0Isr
 *
 * @brief   Port0 ISR
 *
 * @param
 *
 * @return
 ******************************************************************************/
HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
{
  HAL_ENTER_ISR();
  HAL_KEY_SW_PORT0X_IEN &=~HAL_KEY_SW_PORT0X_IENBIT;

  /*
    Clear the CPU interrupt flag for Port_0
    PxIFG has to be cleared before PxIF
  */
  HAL_KEY_CPU_PORT_0_IF = 0;
  HAL_KEY_SW_PORT0X_PXIFG = 0;
  osal_start_timerEx (Hal_TaskID, HAL_KEY_INTERRUPT1_EVENT, KEY_FORBITON_TIME);
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}
/*******************************************************************************
 * @fn      halKeyPort1Isr
 *
 * @brief   Port1 ISR
 *
 * @param
 *
 * @return
 ******************************************************************************/
HAL_ISR_FUNCTION( halKeyPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();
  if (HAL_KEY_SW_1_PXIFG & HAL_KEY_SW_1_BIT)
  {
    halProcessKeyInterrupt();
    HalKeyCurrentKeys = HAL_KEY_SW_1;
  }
  
  if(HAL_KEY_SW_3_PXIFG & HAL_KEY_SW_3_BIT)//p1.3中断标志
  {
    halProcessKeyInterrupt();
    HalKeyCurrentKeys = HAL_KEY_SW_3;

    UART0_RxLength = 0;
    osal_pwrmgr_device(PWRMGR_ALWAYS_ON);
    
    hal_USART0Init();
    
    if(zdappHoldKeys & SW_BYPASS_START)   //进入产测模式
    {
      Elock_Function_Test();
    }
  }
  HAL_KEY_SW_PORT1X_PXIFG = 0;
  HAL_KEY_CPU_PORT_1_IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}
/*******************************************************************************
 * @fn      halKeyPort2Isr
 *
 * @brief   Port2 ISR
 *
 * @param
 *
 * @return
 ******************************************************************************/
HAL_ISR_FUNCTION( halKeyPort2Isr, P2INT_VECTOR )
{
  HAL_ENTER_ISR();
  HAL_KEY_SW_PORT2X_IEN &=~HAL_KEY_SW_PORT2X_IENBIT;

  /*
    Clear the CPU interrupt flag for Port_2
    PxIFG has to be cleared before PxIF
    Notes: P2_1 and P2_2 are debug lines.
  */
  HAL_KEY_SW_PORT2X_PXIFG = 0;
  HAL_KEY_CPU_PORT_2_IF = 0;

  osal_start_timerEx (Hal_TaskID, HAL_KEY_INTERRUPT4_EVENT, KEY_FORBITON_TIME);
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}
/*******************************************************************************
 * @fn      HalKeyInterruptProcessX
 *
 * @brief  - invoke osal_start_timerEx (Hal_TaskID, HAL_KEY_INTERRUPT1_EVENT, 250);
 *
 *           Delay 250ms later will exec HalKeyInterruptProcess1(void)
 * @param
 *
 * @return
 ******************************************************************************/
void HalKeyInterruptProcess1(void)
{

}
void HalKeyInterruptProcess2(void)
{

}
void HalKeyInterruptProcess3(void)
{

}
void HalKeyInterruptProcess4(void)
{
  HAL_KEY_SW_PORT0X_IEN |= HAL_KEY_SW_PORT0X_IENBIT;
  HAL_KEY_SW_PORT1X_IEN |= HAL_KEY_SW_PORT1X_IENBIT;
  HAL_KEY_SW_PORT2X_IEN |= HAL_KEY_SW_PORT2X_IENBIT;
}
/*******************************************************************************
* @fn      HalKeyOnlyMultiClickCheck
*
* @brief
*
*
* @param   none
*
* @return  none
 ******************************************************************************/
void HalKeyOnlyMultiClickCheck(void)
{
  uint8 keys = HalKeyCurrentKeys;
  KeyMultiClicks = HalKeyMultiClickCount;

  if(!(HalKeyRead()&HalKeyCurrentKeys))
  {
    KeyHoldPress = 0;
    if (keys && (pHalKeyProcessFunction))
    {
      (pHalKeyProcessFunction) (keys, HAL_KEY_STATE_NORMAL);
    }
  }
  HalKeyMultiClickCount = 0;
}
/*********************************************************************
* @fn      HalKeyHoldCheck
*
* @brief
*
*
* @param   none
*
* @return  none
*/
void HalKeyHoldCheck(void)
{
  uint8 keys = HalKeyCurrentKeys;
  if(HalKeyRead()&HalKeyCurrentKeys)
  {
    KeyHoldPress = 1;
    if (keys && (pHalKeyProcessFunction))
    {
      (pHalKeyProcessFunction) (keys, HAL_KEY_STATE_NORMAL);
    }
  }
  HalKeyLongHoldActive = 0;
  HalKeyMultiClickCount  = 0;
}
/*********************************************************************
* @fn      HalKeyMultiClickCheck
*
* @brief
*
*
* @param   none
*
* @return  none
*/
void HalKeyMultiClickCheck(void)
{
  uint8 keys = HalKeyCurrentKeys;
  KeyMultiClicks = HalKeyMultiClickCount;
  if(HalKeyMultiClickCount ==1)
  {
    if(!(HalKeyRead()&HalKeyCurrentKeys))
    {
      KeyHoldPress = 0;
      if (keys && (pHalKeyProcessFunction))
      {
        (pHalKeyProcessFunction) (keys, HAL_KEY_STATE_NORMAL);
      }
    }
    else
    {
      KeyMultiClicks  = 0;
      (pHalKeyProcessFunction) (keys, HAL_KEY_STATE_NORMAL);
      osal_start_timerEx (Hal_TaskID, HAL_KEY_HOLD_EVENT, HalKeyHoldValue);
      ZDApp_ResetTimerCancel();
      HalKeyLongHoldActive = 1;
    }
  }
  else
  {
    if(!(HalKeyRead()&HalKeyCurrentKeys))
    {
      KeyHoldPress = 0;
      if (keys && (pHalKeyProcessFunction))
      {
        (pHalKeyProcessFunction) (keys, HAL_KEY_STATE_NORMAL);
      }
    }
  }
  HalKeyMultiClickCount = 0;
}

#else

void HalKeyInit(void){}
void HalKeyConfig(bool interruptEnable, halKeyCBack_t cback){}
uint8 HalKeyRead(void){ return 0;}
void HalKeyPoll(void){}

#endif /* HAL_KEY */
/***************** (C) COPYRIGHT 2015 WuLianGroup ********END OF FIL***********/