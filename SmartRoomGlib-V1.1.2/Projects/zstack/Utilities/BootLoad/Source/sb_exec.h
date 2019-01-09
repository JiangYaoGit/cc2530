/**************************************************************************************************
  Filename:       sb_exec.h
  Revised:        $Date: 2013-09-12 19:32:53 -0700 (Thu, 12 Sep 2013) $
  Revision:       $Revision: 35305 $

  Description:    Serial Bootloader Executive.

  Copyright 2013 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
#ifndef SB_EXEC_H
#define SB_EXEC_H

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_types.h"

/* ------------------------------------------------------------------------------------------------
 *                                          Constants
 * ------------------------------------------------------------------------------------------------
 */

#define SB_RW_BUF_LEN               64

#define SB_BUF_HEADER_SIZE          20 // Current size is about 16, but set larger here just to be safe if it ever changed
#define SB_BUF_SIZE                 80
// The write-page boundary since the write-address is passed as "actual address / flash word size".
#define SB_WPG_SIZE                 (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)
#define SB_ADDR_BLOCK_SIZE          64

#define SB_SOF                      0xAA
#define SB_HANDSHAKE                0xAA

#define SB_MODE_WAIT                0xF8
#define SB_MODE_BOOT               (SB_MODE_WAIT ^ 0xFF)


// Bootloader Serial Interface Subsystem
#define SB_RPC_SYS                  0x3E
#define SB_RPC_IMG                  0x4E
#define SB_RPC_NV                   0x5E
#define SB_RPC_LOCK                 0x6E

// Commands to Bootloader
#define SB_NOTIFICATION             0x00
#define SB_HANDSHAKE_CMD            0x01
#define SB_WRITE_CMD                0x02
#define SB_READ_CMD                 0x03
#define SB_ENABLE_CMD               0x04
#define SB_FORCE_BOOT               0x05
#define SB_FORCE_RUN                0x06
#define SB_FORCE_RESET              0x07
#define SB_BOOT_DLEAY               0x08
// Status codes
#define SB_SUCCESS                  0
#define SB_FAILURE                  1
#define SB_INVALID_FCS              2
#define SB_INVALID_FILE             3
#define SB_FILESYSTEM_ERROR         4
#define SB_ALREADY_STARTED          5
#define SB_NO_RESPOSNE              6
#define SB_VALIDATE_FAILED          7
#define SB_CANCELED                 8

// Serial RX States
#define SB_SOF_STATE                1
#define SB_LEN_STATE                2
#define SB_FRAME_ID_STATE           3
#define SB_CMD_STATE                4
#define SB_DATA_STATE               5
#define SB_FCS_STATE                6


#define SB_IMG_OSET                 (HAL_SB_IMG_ADDR / HAL_FLASH_WORD_SIZE)
#define SB_NV_OSET                  (HAL_SB_NV_ADDR / HAL_FLASH_WORD_SIZE)
#define SB_LOCK_OSET                (HAL_SB_LOCK_ADDR / HAL_FLASH_WORD_SIZE)
#define SB_BOOTLOADER_REVISION     0x01
#define SB_DEVICE_TYPE_2530        0x01
#define SB_DEVICE_TYPE_2531        0x02
#define SB_DEVICE_TYPE_2538        0x03
/* ------------------------------------------------------------------------------------------------
 *                                          Functions
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          sbExec
 *
 * @brief       Boot Loader main executive processing.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if sbCmnd() returns TRUE, indicating that an SB_ENABLE_CMD succeeded;
 *              FALSE otherwise.
 **************************************************************************************************
 */
uint8 sbExec(void);
uint8 sbExecSys(void);
/**************************************************************************************************
 * @fn          sbImgValid
 *
 * @brief       Check validity of the run-code image.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE or FALSE for image valid.
 **************************************************************************************************
 */
uint8 sbImgValid(void);

#endif
