/**
  ******************************************************************************
  * proj    firmware startup project
  * file    demo.c
  * ver     1.0
  * brief   This is a template file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _DEMO_C_

/* Includes ------------------------------------------------------------------*/
#include  "demo.h"  /* mandotary */
#include  "mcu.h"   /* watch dog */
#include  "timer.h" /* timer */

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  int step;
  timer_t timer;  
}
demo_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

demo_t  demo;

/* Private function prototypes -----------------------------------------------*/

static void demo_init(void);
static void demo_run(void);

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    template function
  * param   none
  * retval  none
  */

void  demo_work(void)
{
  /* demo init */
  demo_init();
  /* dead loop */
  for(;;){
    /* feed watch dog */
    feed_watch_dog();
    /* demo run */
    demo_run();
  }
}

static void demo_init(void)
{


}

static void demo_run(void)
{


  switch( demo.step ){

    /* start timer */
    case  0:
      demo.timer.cycle_ticks = 50;  /* 50x10ms */
      timer_start(&demo.timer);
      demo.step ++;
      break;
      
    /* wait timer */
    case  1:
      if( timer_timeout(&demo.timer) ) demo.step ++;
      break;
    
    /* toggle led */
    case  2:
      pin_toggle(PN_LED_RED_N);
      demo.step --;
      break;    
    
    /* error */
    default:
      demo.step = 0;
      break;

  }

}

