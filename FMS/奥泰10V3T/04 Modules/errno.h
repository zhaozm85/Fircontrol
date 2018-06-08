/**
  ******************************************************************************
  * proj    firmware startup project
  * file    ERRNO.h
  * ver     1.0
  * brief   This is a ERRNO file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _ERRNO_H_
#define _ERRNO_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

#define PERRNO(errno,param) perrno(errno,FILE_No,__FILE__,__LINE__,param)
#define PWARNO(warno,param) pwarno(warno,FILE_No,__FILE__,__LINE__,param)

/* Global typedefs ----------------------------------------------------------*/

typedef enum
{
  ERRNO_NONE = 0,
  ERRNO_INVALID_PARAMETERS,
  ERRNO_LTC6802_CRC_ERROR,
  ERRNO_DIVIDE_ZERO,
  ERRNO_CALC_OVERFLOW,
  ERRNO_PRD_WRITE_DISABLED,
  ERRNO_VALUE_REACH_MAX,
}
ERRNO;

typedef enum
{
  WARNO_NONE = 0,
  WARNO_INDEX_OVERFLOW,
}
WARNO;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern void perrno(ERRNO errno, int file_no, char * file, int line, int param);
extern void pwarno(WARNO warno, int file_no, char * file, int line, int param);

/* Re-include control --------------------------------------------------------*/
#endif  /* _ERRNO_H_ */

