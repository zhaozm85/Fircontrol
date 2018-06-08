/**
  ******************************************************************************
  * proj    firmware startup project
  * file    template.h
  * ver     1.0
  * brief   This is a template file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _BAL_H_
#define _BAL_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern void bal_init(void);
extern void bal_proc(void);
extern void bal_reset(void);

extern void bal_set_ext(unsigned long bal);
extern void bal_ena_ext(unsigned char ena); 
extern unsigned char bal_get_ext(void);

extern void bal_ena_int(unsigned char ena);
extern unsigned char bal_get_int(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _TEMPLATE_H_ */

