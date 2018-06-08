/**
  ******************************************************************************
  * proj    firmware startup project - mc9s08dz60
  * file    mcu.h
  * ver     1.0
  * brief   This is a template file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _MCU_H_
#define _MCU_H_

/* Includes ------------------------------------------------------------------*/

#include    "derivative.h" /* include peripheral declarations */

/* Global Macros & Defines --------------------------------------------------*/

#define LED_ACTIVE  0
#define LED_ON      LED_ACTIVE
#define LED_OFF     ~LED_ACTIVE
#define LED_BLUE    PTDD_PTDD2
#define LED_GREEN   PTDD_PTDD3
#define LED_RED     PTDD_PTDD4
#define LED_NA      PTDD_PTDD5

#define LED_ISR_FAULT     LED_NA
#define LED_ISR_RTC       LED_NA
#define LED_ISR_MSCAN_RX  LED_NA

#define LED_BAL     LED_BLUE
#define LED_BMU     LED_GREEN
#define LED_ERROR   LED_NA

/* Global typedefs ----------------------------------------------------------*/

typedef enum
{
  MCU_OK  = 1,
  MCU_NG  = 0,
}
mcu_ret_t;

typedef enum
{
  PN_LED_BLUE_N,
  PN_LED_GREEN_N,
  PN_LED_RED_N,
  PN_SPI_CS_N,
  PN_LTC680X_ENA_N,
  PN_EEPROM_WP,
  PN_CAN_STB,
  PN_FAN_ENA,
  PN_HEAT_ENA,
  PN_VS_ENA_A,
  PN_VS_NCS_A,
  PN_VS_ENA_B,
  PN_VS_NCS_B,
  PN_MAX,
}
gt_ePinNumber;

typedef struct
{
  unsigned char devadd;
  unsigned char * txbuf, * rxbuf;
  unsigned short  txcnt, rxmax;
}
xfer_t;

typedef enum
{
  MSCAN_BRP_500K    = 0,
  MSCAN_BRP_250K    = 1,
  MSCAN_BRP_166K25  = 2,
  MSCAN_BRP_125K    = 3,
  MSCAN_BRP_100K    = 4,
  MSCAN_BRP_MAX     = 5,
}
gt_eCanBaudrate;

typedef enum
{
  MSCAN_IDAM_32x2   = 0,
  MSCAN_IDAM_16x4   = 1,
  MSCAN_IDAM_8x8    = 2,
  MSCAN_IDAM_CLOSED = 3,  
}
mscan_idam_t;

typedef struct
{
  gt_eCanBaudrate brp;
  mscan_idam_t  idam;
  unsigned char idar[8];
  unsigned char idmr[8];
}
mscan_config_t;

typedef struct
{
  CANTIDRSTR  idr;  //  4 bytes
  byte  dsr[8];     //  8 bytes
  CANTDLRSTR  dlr;  //  1 byte
  CANTTBPRSTR bpr;  //  1 byte
}
mscan_txframe_t;

typedef struct
{
	CANRIDRSTR  idr;  //  4 bytes
	byte  dsr[8];     //  8 bytes
	CANRDLRSTR  dlr;  //	1 byte
	byte  reserved;   //  1 byte
	CANRTSRSTR  tsr;  //  2 bytes
}
mscan_rxframe_t;
extern mscan_rxframe_t mscan_rxframe  @ 0x000018A0;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

extern unsigned long tick, dec_tick, rtc_sec;

/* Global function prototypes -----------------------------------------------*/

extern void empty_printf(char*, ...);

extern void init(void);
extern void feed_watch_dog(void);
extern void halt(void);
extern void reset(void);

extern int  pin_high(gt_ePinNumber pn);
extern int  pin_low(gt_ePinNumber pn);
extern int  pin_toggle(gt_ePinNumber pn);

extern void delay_ms(unsigned int ms);

extern int  iic_init(void);
extern int  iic_xfer(xfer_t*);
extern int  iic_reset(void);

extern int  mscan_init(mscan_config_t*);
extern int  mscan_xmit(mscan_txframe_t*);
extern void  mscan_rxcb(mscan_rxframe_t*);
extern int  mscan_reset(void);

extern int  spi_init(void);
extern int  spi_xfer(xfer_t*);
extern int  spi_reset(void);

extern int  adc_init(void);
extern int  adc_conv(unsigned char ch, int delay, int count);
extern int  adc_reset(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _MCU_H_ */

