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
#ifndef _CAN_H_
#define _CAN_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef struct
{
  unsigned long   id;
  unsigned char   dlc;
  unsigned char   data[8];
  unsigned short  ts;
}
can_frame_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern int can_init(void);
extern int can_reset(void);
extern int can_xmit(can_frame_t*);
extern void gf_vCanRxcb(can_frame_t*);

/* Re-include control --------------------------------------------------------*/
#endif  /* _CAN_H_ */

