/**
  ******************************************************************************
  * proj    firmware startup project
  * file    template.c
  * ver     1.0
  * brief   This is a template file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _LOG_C_ 0x43
#define FILE_No _LOG_C_

/* Includes ------------------------------------------------------------------*/
#include  "log.h"  /* mandotary */
#include  "at24c.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef union
{
  unsigned char array[128];
  struct{    
    unsigned int  count;
  }grouped;
}
log_mng_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

log_mng_t log_mng;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    template function
  * param   none
  * retval  none
  */

int log_init(void)
{
  (void)at24c_read_page(0,log_mng.array);
  PRINTF("log cnt: %d\r\n", log_mng.grouped.count);
  return  1;
}

int log_exit(void)
{
  log_mng.grouped.count ++;
  (void)at24c_write_page(0,log_mng.array);
  return  1;
}

int log_printf(char * fmt, ...)
{
  (void)fmt;
  return  1;
}

int log_perrno(int errno, int file, int line, int param)
{
  (void)errno;
  (void)file;
  (void)line;
  (void)param;
  return  NG;
}
