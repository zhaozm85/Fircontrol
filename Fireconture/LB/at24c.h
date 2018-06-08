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

#define OK  1

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

typedef struct
{
  unsigned char devadd;
  unsigned char * txbuf, * rxbuf;
  unsigned short  txcnt, rxmax;
}
xfer_t;
extern int at24c_init(void);
extern int at24c_reset(void);
extern int at24c_read_page(unsigned short page, unsigned char * buffer);
extern int at24c_read_in_page(unsigned short addr, unsigned char * buffer, unsigned short len);
extern int at24c_write_page(unsigned short page, unsigned char * buffer);
extern int at24c_write_in_page(unsigned short addr, unsigned char * buf, unsigned short len);
extern int at24c_read(unsigned short addr, unsigned char * buf, unsigned short len);
extern int at24c_write(unsigned short addr, unsigned char * buf, unsigned short len);

/* Re-include control --------------------------------------------------------*/
/*New*/
extern int iic_init(void);
int iic_reset(void); 
int iic_xfer(xfer_t * x);

#endif  /* _TEMPLATE_H_ */

