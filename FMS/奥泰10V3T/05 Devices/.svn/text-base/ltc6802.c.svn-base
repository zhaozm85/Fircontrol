/**
  ******************************************************************************
  * proj    firmware startup project
  * file    LTC6802.c
  * ver     1.0
  * brief   This is a LTC6802 file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _LTC6802_C_ 0x52
#define FILE_No _LTC6802_C_

/* Includes ------------------------------------------------------------------*/
#include  "ltc6802.h"  /* mandotary */
#include  "mcu.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  unsigned char rxbuf[32];
  unsigned char txbuf[16];
  xfer_t  xfer;
}
ltc6802_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static ltc6802_t  ltc6802;

/* Private function prototypes -----------------------------------------------*/

static unsigned char ltc6802_crc(
  unsigned char len,
  unsigned char * buf
  );

static int ltc6802_write(
  unsigned char   da,
  ltc6802_cmd_t   cmd,
  unsigned char   len,
  void * buf
  );

static int ltc6802_read(
  unsigned char   da,
  ltc6802_cmd_t   cmd,
  unsigned char   len,
  void * buf
  );

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    LTC6802 function
  * param   none
  * retval  none
  */

int ltc6802_init(void){

  ltc6802.xfer.txbuf = ltc6802.txbuf;
  ltc6802.xfer.rxbuf = ltc6802.rxbuf;

  /* init spi */
  (void)spi_init();
  
  /* enable ltc6802 */
  (void)pin_low(PN_VS_ENA_A);
  (void)pin_low(PN_VS_ENA_B);
  return  1;

}

int ltc6802_reset(void){

  /* disable ltc6802 */
  (void)pin_high(PN_VS_ENA_A);
  (void)pin_high(PN_VS_ENA_B);

  /* reset spi */
  return  spi_reset();
  
}

int ltc6802_wrcfg(unsigned char da, ltc6802_cfgreg_t* cfg)
{
  return ltc6802_write(da,LTC6802_CMD_WRCFG,sizeof(ltc6802_cfgreg_t),cfg);
}

int ltc6802_rdcfg(unsigned char da, ltc6802_cfgreg_t* cfg)
{
  return ltc6802_read(da,LTC6802_CMD_RDCFG,sizeof(ltc6802_cfgreg_t),cfg);
}

int ltc6802_rdflg(unsigned char da, ltc6802_flgreg_t* flg)
{
  return ltc6802_read(da,LTC6802_CMD_RDFLG,sizeof(ltc6802_flgreg_t),flg);
}

int ltc6802_stad(unsigned char da, ltc6802_cmd_t cmd)  //  cvad, cvdc, owad, owdc, tmpad
{
  switch(cmd){
    case  LTC6802_CMD_STCVAD:
    case  LTC6802_CMD_STCVDC:
    case  LTC6802_CMD_STOWAD:
    case  LTC6802_CMD_STOWDC:
    case  LTC6802_CMD_STTMPAD:
      return ltc6802_write(da,cmd,0,NULL);
    default:
      PERRNO(ERRNO_INVALID_PARAMETERS,cmd);
      return NG;
  }
}

int ltc6802_rdcv(unsigned char da, ltc6802_cvreg_t* cv)
{
  return ltc6802_read(da,LTC6802_CMD_RDCV,sizeof(ltc6802_cvreg_t),cv);
}

int ltc6802_rdtmp(unsigned char da, ltc6802_tmpreg_t* tmp)
{
  return ltc6802_read(da,LTC6802_CMD_RDTMP,sizeof(ltc6802_tmpreg_t),tmp);
}

int ltc6802_poll(unsigned char da, ltc6802_cmd_t cmd)  //  adc, int
{
  switch(cmd){
    case  LTC6802_CMD_PLADC:
    case  LTC6802_CMD_PLINT:
      return ltc6802_write(da,cmd,0,NULL);
      break;
    default:
      PERRNO(ERRNO_INVALID_PARAMETERS,cmd);
      return NG;
  }
}

static int ltc6802_write(
  unsigned char   da,
  ltc6802_cmd_t   cmd,
  unsigned char   len,
  void * buf){

  int res;

  ltc6802.xfer.txbuf[0] = 0x80|da;
  ltc6802.xfer.txbuf[1] = (unsigned char)cmd;
  (void)memcpy(&ltc6802.xfer.txbuf[2],buf,len);
  ltc6802.xfer.txcnt = 2+len;
  ltc6802.xfer.rxmax = 0;

  res = spi_xfer(&ltc6802.xfer);
  
  return res;

}

static int ltc6802_read(
  unsigned char   da,
  ltc6802_cmd_t   cmd,
  unsigned char   len,
  void * buf)
{

  unsigned char crc;
  int res;
  
  ltc6802.xfer.txbuf[0] = 0x80|da;
  ltc6802.xfer.txbuf[1] = (unsigned char)cmd;
  ltc6802.xfer.txcnt = 2;
  ltc6802.xfer.rxmax = len+1;

  res = spi_xfer(&ltc6802.xfer);

  crc = ltc6802_crc(len+1,ltc6802.xfer.rxbuf);
  /* ok */
  if( crc==0 ){
    (void)memcpy(buf,ltc6802.xfer.rxbuf,len);
    return OK;
  }
  /* error */
  PERRNO(ERRNO_LTC6802_CRC_ERROR,crc);
  return NG;

}

static unsigned char ltc6802_crc(
  unsigned char len,
  unsigned char * buf
  )
{

  unsigned char i,j;
  unsigned long d;  /* lower 17 bits only used */

	d = buf[0];
	d <<= 8;

	for (i = 1; i <= len; i++) {  //  each byte
	  if (i != len) d |= buf[i];
		for (j = 0; j < 8; j++) {   //  one byte
		  if (d & 0x010000) //  msb == 1
		    d ^= 0x010700;  //  poly
		  d <<= 1;
		}
	}

	return (unsigned char)(d>>8);

}
