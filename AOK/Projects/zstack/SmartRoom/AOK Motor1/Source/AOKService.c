#include "AOKService.h"
#include "OSAL_PwrMgr.h"
/********************************************************************************/
uint8 dataReportUp[2];
extern uint8 i;
extern uint8 Data_commnd[30];
/********************************************************************************/

//o¡¥¨ºy¨¦¨´?¡Â
void  JoinNetReponse(void);
void BingdingReponse(void);
void  LeaveNetResponse(void);
void Close_Relay_Safe(void);
/*********************CRCD¡ê?¨¦¡À¨ª*************************************************/
const unsigned char chCRCHTalbe[]=
{
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40
};
const unsigned char chCRCLTalbe[]=
{
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
  0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
  0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
  0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
  0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
  0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
  0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
  0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
  0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
  0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
  0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
  0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
  0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
  0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
  0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
  0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
  0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
  0x41, 0x81, 0x80, 0x40
};
/**************************************************************************/
/* @fn      CRC16
* @brief   D¡ê?¨¦?¦Ì????
* @param   puchMsg      D¡ê?¨¦¨º?¨¨?¨ºy?Y
*          DataLen      D¡ê?¨¦¨ºy?Y3¡è?¨¨       
* @return  none
*/
void  CRC16(uint8 *puchMsg,uint8 DataLen)
{
  Motor.chCRCHi = 0xFF;
  Motor.chCRCLo = 0xFF;
  while(DataLen--)
  {
    Motor.wIndex  = Motor.chCRCLo ^ *puchMsg++;
    Motor.chCRCLo = Motor.chCRCHi ^ chCRCHTalbe[Motor.wIndex];
    Motor.chCRCHi = chCRCLTalbe[Motor.wIndex];
  }
}
/**************************************************************************/
void GetMotorStatus_Event(void)
{   
  UART_SEND_DOWN[3] = 0x01;
  UART_SEND_DOWN[4] = 0x05; 
  UART_SEND_DOWN[5] = 0x01; 
  CRC16((uint8 *)UART_SEND_DOWN,6); //?¨®D¡ê?¨¦?¦Ì
  UART_SEND_DOWN[6] = Motor.chCRCLo;
  UART_SEND_DOWN[7] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 8; 
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}
/*****************************************************************************
* Function Name	  :  QueryMotorStroke
* Description	  :  2¨¦?¡¥¦Ì??¨²¡Á¡ä¨¬?
* Input 	  :  None
* Output	  :  None
* Return	  :  None
*******************************************************************************/
void QueryMotorStroke(void)
{
  //¨¨?¨ª?o¨®2¨¦?¡¥¦Ì??¨²DD3¨¬¨º?¡¤?¡À?¨¦¨¨??
  UART_SEND_DOWN[3] = 0x01;
  UART_SEND_DOWN[4] = 0x02;
  UART_SEND_DOWN[5] = 0x01; 
  CRC16((uint8 *)UART_SEND_DOWN,6); //?¨®D¡ê?¨¦?¦Ì
  UART_SEND_DOWN[6] = Motor.chCRCLo;
  UART_SEND_DOWN[7] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 8; 
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}
/*****************************************************************************/
void JoinNetReponse(void)       // ¨¨?¨ª?¡¤¡ä¨¤?
{
  UART_SEND_DOWN[3] = 0x05;
  UART_SEND_DOWN[4] = 0x01;
  CRC16((uint8 *)UART_SEND_DOWN,5); //?¨®D¡ê?¨¦?¦Ì
  UART_SEND_DOWN[5]= Motor.chCRCLo;
  UART_SEND_DOWN[6] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 7; 
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}

/*****************************************************************************
* Function Name	  :  BingdingReponse
* Description	  :  ¡ã¨®?¡§?a¡ã¨®?¡§¡¤¦Ì??
* Input 	  :  None
* Output	  :  None
* Return	  :  None
*******************************************************************************/
void BingdingReponse(void)
{
  UART_SEND_DOWN[3] = 0x07;
  UART_SEND_DOWN[4] = 0x01;
  CRC16((uint8 *)UART_SEND_DOWN,5); //?¨®D¡ê?¨¦?¦Ì
  UART_SEND_DOWN[5]= Motor.chCRCLo;
  UART_SEND_DOWN[6] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 7; 
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}

/*****************************************************************************/
void JoinNetReponseoK(void)   // ¨¨?¨ª?3¨¦1|¡¤¡ä¨¤?
{
  UART_SEND_DOWN[3] = 0x05;
  UART_SEND_DOWN[4] = 0x03;
  CRC16((uint8 *)UART_SEND_DOWN,5); //?¨®D¡ê?¨¦?¦Ì
  UART_SEND_DOWN[5] = Motor.chCRCLo;
  UART_SEND_DOWN[6] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 7; 
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}
/*****************************************************************************/
void LeaveNetResponse(void)    //¨ª?¨ª?3¨¦1|¡¤¡ä¨¤?
{
  UART_SEND_DOWN[3] = 0x05;
  UART_SEND_DOWN[4] = 0x02;
  CRC16((uint8 *)UART_SEND_DOWN,5); //?¨®D¡ê?¨¦?¦Ì
  UART_SEND_DOWN[5] = Motor.chCRCLo;
  UART_SEND_DOWN[6] = Motor.chCRCHi;
  LEN_UART_SEND_DOWN = 7; 
  hal_UARTWrite(UART_SEND_DOWN, LEN_UART_SEND_DOWN);
}
/******************************************************************************/
void M0042_uartRXFrameAnalysis(void)   //¦Ì¡Á2?¡ä??¨²¨ºy?Y¨º?¦Ì?o¨®??DD¡ä|¨¤¨ª
{
  osal_stop_timerEx(UserApp_TaskID, SAFE_CLOSE_RELAY); 
  uint8 Rec_first=0;
  uint8 j=0;
  for(j=0;j<UART_Buf_Len;j++)
  {
    if(UART_RxBuf[j]==0x55 && UART_RxBuf[j+1]==0xFE &&UART_RxBuf[j+2]==0xFE)
    {
      Rec_first = j;
      break;
    }
  }
  for(j=Rec_first;j<UART_Buf_Len;j++)
  {
    UART_RxBuf[j-Rec_first]=UART_RxBuf[j];
  }
  
  CRC16(UART_RxBuf,UART_Buf_Len-Rec_first-2);
  
  Motor.CRC_buff[0] =  UART_RxBuf[UART_Buf_Len-Rec_first-1];
  Motor.CRC_buff[1] =  UART_RxBuf[UART_Buf_Len-Rec_first-2]; //??D¡ê?¨¦?¦Ì¨¨?3?¨¤¡ä
  
  if((Motor.CRC_buff[0] == Motor.chCRCHi) && (Motor.CRC_buff[1] == Motor.chCRCLo))//D¡ê?¨¦¨ª¡§1y
  {
    /***************************?¨¢?¨¹¨¢?¡¤¦Ì??*************************************/ 
    if(UART_RxBuf[3] == 0x01)  
    {
      if(UART_RxBuf[4] == 0x02) //?¨¢?????¨¹¨¢?¡¤¦Ì??
      {
        if(UART_RxBuf[6] == 0xFF) 
        {
          dataReportUp[0]= 0x00;
          osal_start_timerEx(UserApp_TaskID,GET_MOTOR_STATUS_EVT,200);
        }
        else
        {
          dataReportUp[0]= 0x01;
          dataReportUp[1]= UART_RxBuf[6]; //¡ã¨´¡¤?¡À¨¨2?¨ºy
        }
      }
      /***********************************************************************/
      if(UART_RxBuf[4] == 0x05)   //¨¨??TDD3¨¬¡ê??¨°¨¦?¡À¡§?a1?¨ª¡ê¡Á¡ä¨¬?
      {
        switch(UART_RxBuf[6])
        {
        case 0x00:               //¨ª¡ê
          dataReportUp[1] = 0xFF;
          break;
          
        case 0x01:              //1?
          dataReportUp[1] = 0x64;
          break;
          
        case 0x02:             //?a
          dataReportUp[1] = 0;
          break;
          
        default:
          break;
        }
      }
    }
    /***************************D¡ä?¨¹¨¢?¡¤¦Ì??*************************************/     
    if(UART_RxBuf[3] == 0x02) //????¨¦¨¨¡À????¨°
    {
      dataReportUp[0] = 0x05;
      dataReportUp[1] = UART_RxBuf[5]; //?a¡À??¨¦?¨¹¨º?UART_RxBuf[6]¡ê?
      WZCL_SendDsMessage(dataReportUp,2);   
    }
    /**************************?????¨¹¨¢?¡¤¦Ì??************************************/      
    if(UART_RxBuf[3] == 0x03) 
    {
      switch(UART_RxBuf[4])
      {
      case 0x01:              //¡ä¨°?a¡¤¦Ì??¨¦?¡À¡§
        dataReportUp[0] = 0;
        dataReportUp[1] = 0x64;
        break;
        /*---------------------------------------------------------------------*/   
      case 0x02:             //1?¡À?¡¤¦Ì??¨¦?¡À¡§
        dataReportUp[0] = 0;
        dataReportUp[1] = 0;
        break;
       /*---------------------------------------------------------------------*/    
      case 0x03:            //¨ª¡ê?1¡¤¦Ì??¨¦?¡À¡§
          if(UART_RxBuf[4] == 0x03)
          {
            if(UART_RxBuf[5] == 0xFF)//?TDD3¨¬¨ª¡ê?1
            {
              dataReportUp[0] = 0;
              dataReportUp[1] = 0xFF; 
            }
            if(UART_RxBuf[5] == 0x01) //¨®DDD3¨¬¨ª¡ê?1
            {
              dataReportUp[0] = 1;
              dataReportUp[1] = 0xFF;  
            }
            Motor.Uart_ASk_Flag = 0;
          }
        break;
      /*----------------------------------------------------------------------*/ 
      case 0x04:     //¡ã¨´¡¤?¡À¨¨????¡¤¦Ì??¨ºy?Y
        if((UART_RxBuf[5] != 0xFF) && (Motor.Uart_ASk_Flag == 0x03))
        {
          dataReportUp[0] = 1;
          dataReportUp[1] = UART_RxBuf[5]; 
        }
        if((UART_RxBuf[5] == 0xFF) || (Motor.Uart_ASk_Flag != 0x03))
        {
//          if(Motor.Uart_ASk_Flag == 0x01)
//          {
//            dataReportUp[0] = 1;
//            dataReportUp[1] = 0xFF;  
//            Motor.Uart_ASk_Flag =0;
//          }
//          else
//          {
            dataReportUp[0] = 0;
            dataReportUp[1] = 0xFF; 
//          }
        }
        Motor.Uart_ASk_Flag = 0;
        break;
      /*----------------------------------------------------------------------*/  
      case 0x07:             //¨¦?3yDD3¨¬
        dataReportUp[0] = 0x06;
        dataReportUp[1] = 0x01; 
        break;
      /************************************************************************/  
      default:
        break;
      }
      WZCL_SendDsMessage(dataReportUp,2);    
    }
    /**************************************************************************/   
    if(UART_RxBuf[3] == 0x04)  //¦Ì??¨²???¡é¨ª¡ê?¡Â?¡¥¨¦?¡À¡§¦Ì?¨ºy?Y
    {
      if(UART_RxBuf[4] == 0x02)
      {
        Motor.Position= UART_RxBuf[6]; //¦Ì??¨²????
        Motor.Dir =     UART_RxBuf[7]; //¦Ì??¨²¡¤??¨°
        Motor.ShouLa =  UART_RxBuf[8]; //¦Ì??¨²¨º?¨¤-
        Motor.Status =  UART_RxBuf[9]; //¦Ì??¨²??¨ª¡ê¡Á¡ä¨¬?
        Motor.Travel =  UART_RxBuf[13]; //¦Ì??¨²??¨ª¡ê¡Á¡ä¨¬?
        if(CommndFlag == 0)
        {
          if((Motor.Travel == 0x01) &&(Motor.Position != 0xFF))//DD3¨¬¨°?¨¦¨¨??
          {
            dataReportUp[0] = 0x01; 
            dataReportUp[1] = Motor.Position; 
          }
          if((Motor.Travel != 0x01)  || (Motor.Position == 0xFF))//DD3¨¬¨°?¨¦¨¨??
          {
            //QueryMotorStroke(); 
            dataReportUp[0] = 0x00; 
            dataReportUp[1] = 0xFF;
            //dataReportUp[1] = Motor.Position;  
          }
          WZCL_SendDsMessage(dataReportUp,2);
        }
        else
        {
         CommndFlag =0; 
        }
      }
    }
    /****************************************************************************/     
    // ?¨®¨ª?¨®?¨ª?¨ª???¨¢?
    if(UART_RxBuf[3] == 0x05)
    {
      switch(UART_RxBuf[4])
      {
      case 0x01:
        JoinNetReponse();  //???a¦Ì??¨²¨°??-¨º?¦Ì?¨¦¨º??¨¨?¨ª??¨¹¨¢?
        WZCL_FindAndJoinNetwork();
        break;
        
      case 0x02:
        WZCL_LeaveAndRestoreFactory();
        break;
        
      default:
        break;
      }
    }
   /****************************************************************************/     
    // ¡ã¨®?¡§¨®??a¡ã¨®?¡§??¨¢? 
    if(UART_RxBuf[3] == 0x07)
    {
      switch(UART_RxBuf[4])
      {
      case 0x01:
        BingdingReponse();  //???a¦Ì??¨²¨°??-¨º?¦Ì?¨¦¨º??¨¨?¨ª??¨¹¨¢?
        WZCL_InBindingReqMessage(USERAPP_ENDPOINT1,(cId_t*)bindingInClusterList,USERAPP_IN_BINDINGLIST);//¡ã¨®?¡§2¨´¡Á¡Â
        break;
        
      case 0x02:
        BingdingReponse();
        WZCL_InBindingReqMessage(USERAPP_ENDPOINT1,(cId_t*)bindingInClusterList,USERAPP_IN_BINDINGLIST);//?a¡ã¨®2¨´¡Á¡Â
        break;
        
      default:
        break;
      }
    }
  }
  Motor.Uart_ASk_Flag=0;
  UART_Buf_Len = 0;
  osal_start_timerEx( UserApp_TaskID,SAFE_CLOSE_RELAY,90000);
}
void Close_Relay_Safe(void)
{
    static uint8 Stopsafe[9]={0x55,0xFE,0xFE,0x03,0x03,0x01,0x25,0x12};
    hal_UARTWrite(Stopsafe,8);
}