/**
  ******************************************************************************
  * proj    firmware startup project
  * file    timer.h
  * ver     1.0
  * brief   This is a template file for all H files.
  ------------------------------------------------------------------------------
  * 2011.11.20  created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _TIMER_H_
#define _TIMER_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef struct
{
  unsigned long cycle_ticks;
  unsigned long start_tick;
}
timer_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

//  called by system timer handler, such as 10 ms timer interrupt
extern  void  timer_tickup(void);

//  called by user to start a timer
extern  void  timer_start(timer_t*);
//  called by user to check if time out reached
extern  int   timer_timeout(timer_t*);

/* Re-include control --------------------------------------------------------*/
#endif  /* _TIMER_H_ */
