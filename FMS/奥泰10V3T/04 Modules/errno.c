/**
  ******************************************************************************
  * proj    firmware startup project
  * file    ERRNO.c
  * ver     1.0
  * brief   This is a ERRNO file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _ERRNO_C_ 0x45
#define FILE_No _ERRNO_C_

/* Includes ------------------------------------------------------------------*/
#include  "errno.h"  /* mandotary */
#include  "log.h"
#include  "can.h"
#include  "mcu.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    ERRNO function
  * param   none
  * retval  none
  */

void perrno(ERRNO errno, int file_no, char * file, int line, int param)
{
  LED_RED = LED_ON;

  //  sci
  PRINTF("[E]%d @ %s(%d): %d\r\n",errno,file,line,param);

  //  can
  {
    static can_frame_t frame;
    frame.id = 0x000;
    frame.dlc = 8;
    frame.data[0] = (unsigned char)(errno>>8);
    frame.data[1] = (unsigned char)(errno>>0);
    frame.data[2] = (unsigned char)(file_no>>8);
    frame.data[3] = (unsigned char)(file_no>>0);
    frame.data[4] = (unsigned char)(line>>8);
    frame.data[5] = (unsigned char)(line>>0);
    frame.data[6] = (unsigned char)(param>>8);
    frame.data[7] = (unsigned char)(param>>0);
 //   (void)can_xmit(&frame);
  }

  //  log
  (void)log_printf("[E]%d,%d,%d,%d\r\n",errno,file,line,param);
 
  LED_RED = LED_OFF;
}

void pwarno(WARNO warno, int file_no, char * file, int line, int param)
{
  LED_RED = LED_ON;

  //  sci
  PRINTF("[E]%d @ %s(%d): %d\r\n",errno,file,line,param);

  //  can
  {
    static can_frame_t frame;
    frame.id = 0x000;
    frame.dlc = 8;
    frame.data[0] = (unsigned char)(warno>>8);
    frame.data[1] = (unsigned char)(warno>>0);
    frame.data[2] = (unsigned char)(file_no>>8);
    frame.data[3] = (unsigned char)(file_no>>0);
    frame.data[4] = (unsigned char)(line>>8);
    frame.data[5] = (unsigned char)(line>>0);
    frame.data[6] = (unsigned char)(param>>8);
    frame.data[7] = (unsigned char)(param>>0);
//    (void)can_xmit(&frame);
  }

  //  log
  (void)log_printf("[W]%d,%d,%d,%d\r\n",warno,file,line,param);
 
  LED_RED = LED_OFF;
}

