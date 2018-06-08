/**
  ******************************************************************************
  * proj    firmware startup project
  * file    MNG.h
  * ver     1.0
  * brief   This is a MNG file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _MNG_H_
#define _MNG_H_

/* Includes ------------------------------------------------------------------*/

#include  "can.h"

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern void mng_crx_save(can_frame_t*);
extern unsigned char mng_crx_proc(void);

extern void mng_info(void);

extern void mng_init(void);
extern void mng_reset(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _MNG_H_ */

