/**
  ******************************************************************************
  * proj    firmware startup project
  * file    NTC.h
  * ver     1.0
  * brief   This is a NTC file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _NTC_H_
#define _NTC_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

#define NTC_TYPE_CWF4   0
#define NTC_TYPE_EPCOS  1
#define NTC_TYPE_MURATA 2

/* Global typedefs ----------------------------------------------------------*/

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern signed short ntc_res_to_temp(unsigned long res, unsigned char type);

/* Re-include control --------------------------------------------------------*/
#endif  /* _NTC_H_ */

