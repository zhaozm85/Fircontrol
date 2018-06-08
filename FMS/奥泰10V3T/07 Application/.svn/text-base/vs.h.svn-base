/**
  ******************************************************************************
  * proj    firmware startup project
  * file    VS.h
  * ver     1.0
  * brief   This is a VS file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _VS_H_
#define _VS_H_

/* Includes ------------------------------------------------------------------*/

#include  "bmu.h"

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef struct
{
  //  bit15 : 1 - invalid, 0 - valid
  unsigned short  volt[12]; //  1mV/bit
  unsigned short  vtemp1;   //  1mV/bit
  unsigned short  vtemp2;   //  1mV/bit
  unsigned short  itemp;    //  -40.0C~
  unsigned short  balance;  //  000~FFF
}
vs_data_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

extern vs_data_t vs_data[LTC6802_MAX];

/* Global function prototypes -----------------------------------------------*/

extern void vs_init(void);
extern void vs_proc(void);
extern void vs_reset(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _VS_H_ */

