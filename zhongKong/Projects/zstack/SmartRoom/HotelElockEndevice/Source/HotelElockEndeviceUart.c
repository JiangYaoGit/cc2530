/***************************************************************************************************
  Filename:       UserUart.c
  Revised:        $Date: 2009-03-12 16:25:22 -0700 (Thu, 12 Mar 2009) $
  Revision:       $Revision: 19404 $

  Description:  This module handles anything dealing with the serial port.

  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "OSAL.h"
#include "OSAL_Memory.h"
#include "DeviceConfig.h"
#include "ZDApp.h"
#include "UserUart.h"
extern uint8  ProcessUartCMD( uint8 *datBuf ,uint16 length);

uint8  UART0_RxBuf[600];
uint16 UART0_RxLength = 0;//zhoujiong20150811

/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/
/*******************************************************************************
 * @fn      hal_USART0Init
 *
 * @brief   ����0��ʼ��
 *
 * @param   none
 *
 * @return  none
 */
void hal_USART0Init(void)
{ 
  P0INP &= ~0x0c;
  
  P0SEL |= 0x04; //P0.2 peripherel
  P0SEL |= 0x08; //P0.3 peripherel
  
  U0BAUD = 59;
  U0UCR = 0x02;
  
#if defined(OUBAO)  //OUBAO 1200
  U0GCR = 0x05;
#else
  
  U0GCR = 0x09;
#endif
  
  U0CSR = 0x82;
  IEN0 |= 0x04;
  
  EA =1;
  U0CSR |= 0x40;
  U0CSR &= ~(0x04);//�����־λ
}
/*******************************************************************************
 * @fn      hal_UARTWrite
 *
 * @brief   ����0��������
 *
 * @param   uint8 *datBuf, uint8 len
 *
 * @return  none
 */
void hal_UARTWrite ( uint8 *datBuf, uint8 len)
{
  uint8 datCnt;

  for(datCnt = 0; datCnt < len; datCnt++)
  {
    while(U0CSR & 0x01);
    U0DBUF = *datBuf;
    while(0 == (U0CSR &(1<<1)));	//ֱ���������
    U0CSR |= (0x02);		//�����־λ
    datBuf++;
  }
}
/*******************************************************************************
 * @fn      HAL_ISR_FUNCTION( halUSART0RxIsr, URX0_VECTOR )
 *
 * @brief   ����0�жϺ���
 *
 * @param   UART0_RxLength  UART0_RxBuf[]
 *
 * @return  none
 */
HAL_ISR_FUNCTION(halUSART0RxIsr, URX0_VECTOR)
{
  //EA = 0;
  TCON &= ~(0x08);                              //08
  U0CSR &= ~(1<<2);				//�����־λ
  UART0_RxBuf[UART0_RxLength] = U0DBUF;	
  UART0_RxLength++;
  
  if(UART0_RxLength > 599)
  {
    UART0_RxLength=0;
  }
  osal_start_timerEx( UserApp_TaskID,UART_RX_EVT,100);
  //EA = 1;
}
/***************************************************************************************************
***************************************************************************************************/
