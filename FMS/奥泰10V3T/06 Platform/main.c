/**
  ******************************************************************************
  * proj    firmware startup project
  * file    main.c
  * ver     1.0
  * brief   This is a template file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _MAIN_C_  0x61
#define FILE_No _MAIN_C_

/* Includes ------------------------------------------------------------------*/
#include  "mcu.h"
#include  "test.h"
#include  "bmu.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    template function
  * param   none
  * retval  none
  */

volatile unsigned int n @ 0x1070;

void main(void)
{

  /* basic init : xtal, pins... */
  init();
  
  DBG_PRINTF("\r\n\r\nstart up %d for %02Xh\r\n", n, SRS);
  n ++;

  /* tests */
  test();
  
  /* works */
 gf_vBmuMain();

  /* trap if work returns */
  halt();

}

