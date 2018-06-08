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
#include "MC9S08DZ60.h"
#include "FirM.h"
#include "MCU_IO.h"

/* Private Macros & Defines --------------------------------------------------*/
/*
#ifndef DEBUG_DEV_AT24C
#define DEBUG_DEV_AT24C 0
#endif

#if DEBUG_DEV_AT24C
#define AT24C_PRINTF  PRINTF
#else
#define AT24C_PRINTF  empty_printf
#endif
 */
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
   iic_init();
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
  (void)pin_low(PN_EEPROM_WP);;
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

 // AT24C_PRINTF("at24c read: %u, %Xh, %u\r\n", addr, (unsigned short)buf,len);

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

 int at24c_write_in_page(unsigned short addr, unsigned char * buf, unsigned short len)
{

  unsigned char i;

 // AT24C_PRINTF("at24c wip: %u, %Xh, %u\r\n", addr, (unsigned short)buf,len);

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
  delay_ms(15);

  /* set wp again */
  (void)pin_high(PN_EEPROM_WP);
  
  /* done */
  return  len;

}
//
int at24c_read_in_page(unsigned short addr, unsigned char * buffer, unsigned short len){

  unsigned char i;

  /* prepare data */
  //page *= 128;
  at24c.txbuf.grouped.wa1 = (unsigned char)(addr>>8);
  at24c.txbuf.grouped.wa2 = (unsigned char)addr;
  at24c.xfer.devadd = DEVICE_ADDRESS;
  at24c.xfer.txcnt = 2;
  at24c.xfer.txbuf = at24c.txbuf.buffer;
  at24c.xfer.rxmax = len;
  at24c.xfer.rxbuf = buffer;

  /* transfer iic data */
  (void)iic_xfer(&at24c.xfer);

  /* done */
  return  1;

}
//

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
/*NEW*/
int iic_init(void)
{
  // 100Kbps
  /* ### Init_IIC init code */
  /* IICC2: GCAEN=0,ADEXT=0,AD10=0,AD9=0,AD8=0 */
  IICC2 = 0x00;                                      
  /* IICA: AD7=0,AD6=0,AD5=0,AD4=0,AD3=0,AD2=0,AD1=0 */
  IICA = 0x00;                                      
  /* IICF: MULT1=1,MULT0=0,ICR5=0,ICR4=0,ICR3=0,ICR2=1,ICR1=1,ICR0=1 */
  IICF = 0x87;                                      
  /* IICS: TCF=1,IAAS=0,BUSY=0,ARBL=1,SRW=0,IICIF=1,RXAK=0 */
  IICS = 0x92;                         /* Clear the interrupt flags */
  /* IICC: IICEN=1 */
  IICC |= (unsigned char)0x80;                      
  /* IICC: IICEN=1,IICIE=0,MST=0,TX=1,TXAK=0,RSTA=0 */
  IICC = 0x90;                                      

  /* done */
 // IIC_PRINTF("iic init done\r\n");
  return  OK;
}

int iic_reset(void)
{
  IICC = 0x00;
  IICC2 = 0x00;
  return  OK;
}

int iic_xfer(xfer_t * x)
{

  unsigned short i;
  unsigned char iics;

 // __RESET_WATCHDOG();

  /* check iic bus status */
  iics = IICS;
  if( iics==0x20) {
    (void)iic_reset();
    (void)iic_init();
    return -1;
  }

  /* before tx start */
  IICC_TXAK = 0;

  /* generate a start */
  IICC |= 0x30;   // And generate START condition;
  
  /* tx */
  if( x->txcnt>0 ){

    /* send address, with WRITE direction */
   // IIC_PRINTF("T");
    IICD = (x->devadd<<1);    // Address the slave and set up for master transmit;
    while (!IICS_IICIF);  // wait until IBIF;
    IICS_IICIF=1; // clear the interrupt event flag;
    while(IICS_RXAK); // check for RXAK;

    /* send data buffer */  
    for(i=0;i<x->txcnt;i++){
    //  IIC_PRINTF(">");
      IICD = x->txbuf[i];    // Address the slave and set up for master transmit;
    //  IIC_PRINTF("%02X",x->txbuf[i]);
      while (!IICS_IICIF);  // wait until IBIF;
      IICS_IICIF=1; // clear the interrupt event flag;
      while(IICS_RXAK); // check for RXAK;
    }

    /* stop of write */
    if( x->rxmax==0 ){
    //  IIC_PRINTF("P");
      IICC_MST = 0;
    }
    /* restart for read */
    else{
    //  IIC_PRINTF("s");
      IICC_RSTA = 1;
    }

  }

  /* rx */
  if( x->rxmax>0 ){
  
    /* send address, with READ direction */
  //  IIC_PRINTF("T");
    IICD = (x->devadd<<1)+1;    // Address the slave and set up for master transmit;
    while (!IICS_IICIF);  // wait until IBIF;
    IICS_IICIF=1; // clear the interrupt event flag;
    while(IICS_RXAK); // check for RXAK;

    /* start first byte */
    IICC_TX = 0;
    IICC_TXAK = 0;

    //IIC_PRINTF("R");
    x->rxbuf[0] = IICD; //  dummy read to start receive

    /* receive data bytes */
    for(i=0;i<x->rxmax-1;i++){

      /* wait done */
      while( !IICS_IICIF );
      IICS_IICIF = 1; //  clear

      /* set next ack */
      if( i==x->rxmax-2 ) IICC_TXAK = 1;
      /* read and save data and start the next */
    //  IIC_PRINTF("<");
      x->rxbuf[i] = IICD;
  //    IIC_PRINTF("%02X",x->rxbuf[i]);

    }

    /* receive last byte */
    while( !IICS_IICIF );
    IICS_IICIF = 1;
    /* stop */
  //  IIC_PRINTF("<");
    IICC_MST = 0;
    /* save last data */
    x->rxbuf[i] = IICD;
   // IIC_PRINTF("%02X",x->rxbuf[i]);
  //  IIC_PRINTF("P");

  }
  
//  IIC_PRINTF("\r\n");

  /* done */
  return  OK;
}
