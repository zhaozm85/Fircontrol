/**
  ******************************************************************************
  * proj    firmware startup project
  * file    BMU.h
  * ver     1.0
  * brief   This is a BMU file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _BMU_H_
#define _BMU_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

#define LTC6802_MAX 2
#define FANSTARTTEMP    5      //fan start temp for temp difference 5
#define FANMINSTARTTEMP      38 + 40     //fan start temp for temp 38
#define FANSTOPTEMP    2      //fan start temp for temp difference 2
#define FANMAXSTOPTEMP      28 + 40     //fan start temp for temp 28

/* Global typedefs ----------------------------------------------------------*/

typedef enum
{
  BMU_MODE_NONE = 0,
  BMU_MODE_CONFIG,
  BMU_MODE_NORMAL,
}
gt_eBmuMode;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/
//volatile unsigned char fanStatus;

/* Global function prototypes -----------------------------------------------*/

extern void gf_vBmuMain(void);
//unsigned char gf_fanDrive(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _BMU_H_ */

