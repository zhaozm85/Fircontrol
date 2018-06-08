/**
  ******************************************************************************
  * proj    firmware startup project
  * file    LTC6802.h
  * ver     1.0
  * brief   This is a LTC6802 file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _LTC6802_H_
#define _LTC6802_H_

/* Includes ------------------------------------------------------------------*/

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef enum
{
  LTC6802_CMD_WRCFG   = 0x01,
  LTC6802_CMD_RDCFG   = 0x02,
  LTC6802_CMD_RDCV    = 0x04,
  LTC6802_CMD_RDFLG   = 0x06,
  LTC6802_CMD_RDTMP   = 0x08,
  LTC6802_CMD_STCVAD  = 0x10,  /* normal ad */
  LTC6802_CMD_STCVDC  = 0x60,  /* normal ad, poll status, discharge permitted */
  LTC6802_CMD_STOWAD  = 0x20,  /* open-wired ad */
  LTC6802_CMD_STOWDC  = 0x70,  /* open-wired ad, poll status, discharge permitted */
  LTC6802_CMD_STTMPAD = 0x30,  /* temprature ad */
  LTC6802_CMD_PLADC   = 0x40,  /* poll adc */
  LTC6802_CMD_PLINT   = 0x50,  /* poll interrupt */
}
ltc6802_cmd_t;

typedef struct
{
  /* cfgr0 */
  volatile unsigned int   cdc:3;
  volatile unsigned int   cell10:1;
  volatile unsigned int   lvlpl:1;
  volatile unsigned int   gpio1:1;
  volatile unsigned int   gpio2:1;
  volatile unsigned int   wdt:1;
  /* cfgr1 */
  volatile unsigned char  dcc8_1;
  /* cfgr2 */
  volatile unsigned int   dcc12_9:4;
  volatile unsigned int   mci4_1:4;
  /* cfgr3 */
  volatile unsigned char  mci12_5;
  /* cfgr4 */
  volatile unsigned char  vuv;
  /* cfgr5 */
  volatile unsigned char  vov;
}
ltc6802_cfgreg_t;

typedef struct
{
  struct
  {
    volatile unsigned char  c0v7_0;
    volatile unsigned int   c0v11_8:4;
    volatile unsigned int   c1v3_0:4;
    volatile unsigned char  c1v11_4;
  }
  group[6];
}
ltc6802_cvreg_t;

typedef struct
{
  /* byte 0 */
  volatile unsigned int c1uv:1;
  volatile unsigned int c1ov:1;
  volatile unsigned int c2uv:1;
  volatile unsigned int c2ov:1;
  volatile unsigned int c3uv:1;
  volatile unsigned int c3ov:1;
  volatile unsigned int c4uv:1;
  volatile unsigned int c4ov:1;
  /* byte 1 */
  volatile unsigned int c5uv:1;
  volatile unsigned int c5ov:1;
  volatile unsigned int c6uv:1;
  volatile unsigned int c6ov:1;
  volatile unsigned int c7uv:1;
  volatile unsigned int c7ov:1;
  volatile unsigned int c8uv:1;
  volatile unsigned int c8ov:1;
  /* byte 2 */
  volatile unsigned int c9uv:1;
  volatile unsigned int c9ov:1;
  volatile unsigned int c10uv:1;
  volatile unsigned int c10ov:1;
  volatile unsigned int c11uv:1;
  volatile unsigned int c11ov:1;
  volatile unsigned int c12uv:1;
  volatile unsigned int c12ov:1;
}
ltc6802_flgreg_t;

typedef struct
{
  volatile unsigned char  etmp1_7_0;
  volatile unsigned int   etmp1_11_8:4;
  volatile unsigned int   etmp2_3_0:4;
  volatile unsigned char  etmp2_11_4;
  volatile unsigned char  itmp_7_0;
  volatile unsigned int   itmp_11_8:4;
  volatile unsigned int   thsd:1; /* thermal shutdown status */
  volatile unsigned int   rev:3;  /* device revision code */
}
ltc6802_tmpreg_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

/* Global function prototypes -----------------------------------------------*/

extern int ltc6802_init(void);
extern int ltc6802_reset(void);

extern int ltc6802_wrcfg(unsigned char da, ltc6802_cfgreg_t* cfg);
extern int ltc6802_rdcfg(unsigned char da, ltc6802_cfgreg_t* cfg);

extern int ltc6802_rdflg(unsigned char da, ltc6802_flgreg_t*);

extern int ltc6802_stad(unsigned char da, ltc6802_cmd_t);  //  cvad, cvdc, owad, wdc, tmpad
extern int ltc6802_rdcv(unsigned char da, ltc6802_cvreg_t*);
extern int ltc6802_rdtmp(unsigned char da, ltc6802_tmpreg_t*);

extern int ltc6802_poll(unsigned char da, ltc6802_cmd_t);  //  adc, int

/* Re-include control --------------------------------------------------------*/
#endif  /* _LTC6802_H_ */

