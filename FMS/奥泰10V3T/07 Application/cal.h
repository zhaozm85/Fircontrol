/**
  ******************************************************************************
  * proj    firmware startup project
  * file    CAL.h
  * ver     1.0
  * brief   This is a CAL file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _CAL_H_
#define _CAL_H_

/* Includes ------------------------------------------------------------------*/
#include  "bmu.h"

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef struct
{
  //  volt
  unsigned short  volt[12*LTC6802_MAX];
  unsigned short  vmax;
  unsigned short  vMaxLow;
  unsigned short  vMaxHigh;
  unsigned short  vmin;
  unsigned short  vMinLow;
  unsigned short  vMinHigh;
  unsigned short  vave;
  unsigned short  vAveLow;
  unsigned short  vAveHigh;
  unsigned long   vsum;
  unsigned long   vSumLow;
  unsigned long   vSumHigh;
  unsigned char   ivmax;
  unsigned char   ivMaxLow;     //The max voltage unmber of cell 1 to 12 
  unsigned char   ivMaxHigh;    //The max voltage unmber of cell 12 to 24
  unsigned char   ivmin;
  unsigned char   ivMinLow;     //The min voltage unmber of cell 1 to 12 
  unsigned char   ivMinHigh;    //The min voltage unmber of cell 12 to 24     
  //  temp
  unsigned short  temp[24];
  unsigned short  tmax;
  unsigned short  tMaxLow;
  unsigned short  tMaxHigh;
  unsigned short  tmin;
  unsigned short  tMinLow;
  unsigned short  tMinHigh;
  unsigned short  tave;
  unsigned short  tAveLow;
  unsigned short  tAveHigh;
  unsigned long   tsum;
  unsigned long   tSumLow;
  unsigned long   tSumHigh;
  unsigned char   itmax;
  unsigned char   itMaxLow;     //The max temp unmber of cell 1 to 12 
  unsigned char   itMaxHigh;    //The max temp unmber of cell 12 to 24 
  unsigned char   itmin;
  unsigned char   itMinLow;     //The min temp unmber of cell 1 to 12 
  unsigned char   itMinHigh;    //The min temp unmber of cell 12 to 24 
  unsigned short  temp_sorted[24];
  //  other temp
  unsigned short  bal_temp;
  unsigned short  main_temp;
}
cal_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

extern cal_t cal;

/* Global function prototypes -----------------------------------------------*/

extern void cal_init(void);
extern void cal_proc(void);
extern void cal_reset(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _CAL_H_ */

