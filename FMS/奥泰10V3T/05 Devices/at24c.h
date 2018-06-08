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
#ifndef _AT24C_H_
#define _AT24C_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern int at24c_init(void);
extern int at24c_reset(void);
extern int at24c_read_page(unsigned short page, unsigned char * buffer);
extern int at24c_write_page(unsigned short page, unsigned char * buffer);

extern int at24c_read(unsigned short addr, unsigned char * buf, unsigned short len);
extern int at24c_write(unsigned short addr, unsigned char * buf, unsigned short len);

/* Re-include control --------------------------------------------------------*/
#endif  /* _TEMPLATE_H_ */

