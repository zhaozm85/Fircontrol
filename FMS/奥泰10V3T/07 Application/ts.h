/**
  ******************************************************************************
  * proj    firmware startup project
  * file    TS.h
  * ver     1.0
  * brief   This is a TS file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _TS_H_
#define _TS_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef struct
{
  unsigned short temp[24];  //  -40.0C~
  unsigned short mcu_temp;  //  -40.0C~
}
ts_data_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

extern ts_data_t ts_data;

/* Global function prototypes -----------------------------------------------*/

extern void ts_init(void);
extern void ts_proc(void);
extern void ts_reset(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _TS_H_ */

