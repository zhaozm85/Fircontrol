/**
  ******************************************************************************
  * proj    firmware startup project
  * file    timer.c
  * ver     1.0
  * brief   This is a template file for all C files.
  ------------------------------------------------------------------------------
  * 2011.11.20  created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _TIMER_C_ 0x42
#define FILE_No _TIMER_C_

/* Includes ------------------------------------------------------------------*/
#include  "timer.h"  /* mandotary */

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

unsigned long  timer_tick = 0;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    timer interrupt hander, called by timer isr
  * param   none
  * retval  none
  */

void  timer_tickup(void)
{
  timer_tick ++;
}

/**
  * bref    start a timer
  * param   pointer of a timer
  * retval  none
  */

void  timer_start(timer_t* timer)
{
  timer->start_tick = timer_tick;
}

/**
  * bref    check if time out
  * param   pointer of a timer
  * retval  1 timeout, 0 not timeout
  */

int   timer_timeout(timer_t* timer)
{
  /* save now tick in case new interrupt occurs */
  unsigned long now_tick = timer_tick;
  /* get passed ticks */
  unsigned long passed_ticks = now_tick - timer->start_tick;
  /* not time out */
  if( passed_ticks < timer->cycle_ticks ) return 0;
  /* new start tick for next cyclec */
  timer->start_tick += timer->cycle_ticks;
  /* ok */
  return  1;
}

