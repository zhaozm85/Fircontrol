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
#define _AT24C_C_ 0x51
#define FILE_No _AT24C_C_

/* Includes ------------------------------------------------------------------*/
#include  "at24c.h"  /* mandotary */
#include  "mcu.h"     /* iic, xfer */

/* Private Macros & Defines --------------------------------------------------*/

#ifndef DEBUG_DEV_AT24C
#define DEBUG_DEV_AT24C 0
#endif

#if DEBUG_DEV_AT24C
#define AT24C_PRINTF  PRINTF
#else
#define AT24C_PRINTF  empty_printf
#endif

#define DEVICE_ADDRESS  0x50  /* 7 bits */

/* Private typedefs ----------------------------------------------------------*/

typedef union
{
  struct{    
    volatile unsigned char wa1;
    volatile unsigned char wa2;
    volatile unsigned char data[128];
  }grouped;
  unsigned char  buffer[2+128];
}
at24c_txbuf_t;

typedef struct
{
  at24c_txbuf_t  txbuf;
  xfer_t  xfer;
}
at24c_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

at24c_t at24c;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    template function
  * param   none
  * retval  none
  */

int at24c_init(void) {
  return iic_init();
}

int at24c_reset(void){
  return iic_reset();
}

int at24c_write_page(unsigned short page, unsigned char * buffer){

  unsigned char i;

  if( page>=512 ) return -1;

  /* prepare data */
  page *= 128;
  at24c.txbuf.grouped.wa1 = page/256;
  at24c.txbuf.grouped.wa2 = page%256;
  for(i=0;i<128;i++) at24c.txbuf.grouped.data[i] = buffer[i];
  at24c.xfer.devadd = DEVICE_ADDRESS;
  at24c.xfer.txcnt = 2+128;
  at24c.xfer.txbuf = at24c.txbuf.buffer;
  at24c.xfer.rxmax = 0;
  at24c.xfer.rxbuf = 0;
  
  /* release wp and delay 1 ms */
  (void)pin_low(PN_EEPROM_WP);
  delay_ms(1);
  
  /* transfer iic data */
  (void)iic_xfer(&at24c.xfer);
  
  /* wait until program done, should done in 5 ms */
  delay_ms(10);

  /* set wp again */
  (void)pin_high(PN_EEPROM_WP);
  
  /* done */
  return  1;

}

int at24c_read_page(unsigned short page, unsigned char * buffer){

  if( page>=512 ) return -1;

  /* prepare data */
  page *= 128;
  at24c.txbuf.grouped.wa1 = page/256;
  at24c.txbuf.grouped.wa2 = page%256;
  at24c.xfer.devadd = DEVICE_ADDRESS;
  at24c.xfer.txcnt = 2;
  at24c.xfer.txbuf = at24c.txbuf.buffer;
  at24c.xfer.rxmax = 128;
  at24c.xfer.rxbuf = buffer;

  /* transfer iic data */
  (void)iic_xfer(&at24c.xfer);

  /* done */
  return  1;

}

int at24c_read(unsigned short addr, unsigned char * buf, unsigned short len)
{

  AT24C_PRINTF("at24c read: %u, %Xh, %u\r\n", addr, (unsigned short)buf,len);

  /* prepare data */
  at24c.txbuf.grouped.wa1 = (unsigned char)(addr>>8);
  at24c.txbuf.grouped.wa2 = (unsigned char)addr;
  at24c.xfer.devadd = DEVICE_ADDRESS;
  at24c.xfer.txcnt = 2;
  at24c.xfer.txbuf = at24c.txbuf.buffer;
  at24c.xfer.rxbuf = buf;
  at24c.xfer.rxmax = len;
  
  /* transfer iic data */
  (void)iic_xfer(&at24c.xfer);
  
  /* done */
  return 1;

}

static int at24c_write_in_page(unsigned short addr, unsigned char * buf, unsigned short len)
{

  unsigned char i;

  AT24C_PRINTF("at24c wip: %u, %Xh, %u\r\n", addr, (unsigned short)buf,len);

  /* prepare data */
  at24c.txbuf.grouped.wa1 = (unsigned char)(addr>>8);
  at24c.txbuf.grouped.wa2 = (unsigned char)addr;
  for(i=0;i<len;i++) at24c.txbuf.grouped.data[i] = buf[i];
  at24c.xfer.devadd = DEVICE_ADDRESS;
  at24c.xfer.txcnt = 2+len;
  at24c.xfer.txbuf = at24c.txbuf.buffer;
  at24c.xfer.rxmax = 0;
  at24c.xfer.rxbuf = 0;
  
  /* release wp and delay 1 ms */
  (void)pin_low(PN_EEPROM_WP);
  delay_ms(10);
  
  /* transfer iic data */
  (void)iic_xfer(&at24c.xfer);
  
  /* wait until program done, should done in 5 ms */
  delay_ms(100);

  /* set wp again */
  (void)pin_high(PN_EEPROM_WP);
  
  /* done */
  return  len;

}

int at24c_write(unsigned short addr, unsigned char * buf, unsigned short len)
{

  unsigned short txcnt;
  
  txcnt = ((addr&0xFF80)+0x0080)-addr;

  /* each page */
  while( txcnt<len ){
    (void)at24c_write_in_page(addr,buf,txcnt);
    addr += txcnt;
    buf += txcnt;
    len -= txcnt;
    txcnt = 128;

  }

  /* last page */
  (void)at24c_write_in_page(addr,buf,len);

  /* done */
  return 1;

}
