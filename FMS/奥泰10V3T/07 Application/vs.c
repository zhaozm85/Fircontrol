/**
  ******************************************************************************
  * proj    firmware startup project
  * file    VS.c
  * ver     1.0
  * brief   This is a VS file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _VS_C_  0x73
#define FILE_No _VS_C_

/* Includes ------------------------------------------------------------------*/
#include  "vs.h"  /* mandotary */
#include  "ltc6802.h"
#include  "mcu.h"
#include  "sc.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  ltc6802_cfgreg_t  cfg;
  ltc6802_cvreg_t   cv;
  ltc6802_tmpreg_t  tmp;
}
vs_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

vs_t  vs;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

vs_data_t vs_data[LTC6802_MAX];

/* Functions -----------------------------------------------------------------*/

/**
  * bref    VS function
  * param   none
  * retval  none
  */

static void vs_clear(unsigned char dev)
{
  unsigned char i;
  vs_data[dev].itemp = 0x8000;
  vs_data[dev].vtemp1 = 0x8000;
  vs_data[dev].vtemp2 = 0x8000;
  for(i=0;i<12;i++) vs_data[dev].volt[i] = 0x8000;
}

static void vs_scan(unsigned char dev)
{
  int res;

  gt_ePinNumber cs = gv_sSC.sVS.saLtc6802[dev].eLtc6802SpiCsN;
  unsigned char da = gv_sSC.sVS.saLtc6802[dev].bDeviceAddress;

  /* set config */
  vs.cfg.cdc = 0x01;
  vs.cfg.cell10 = 0;
  vs.cfg.lvlpl = 0;
  vs.cfg.gpio1 = 1;
  vs.cfg.gpio2 = 1;
  vs.cfg.wdt = 1;
  vs.cfg.dcc8_1 = 0;
  vs.cfg.dcc12_9 = 0;
  {
    unsigned short bal;
    bal = vs_data[dev].balance;
    if( (bal&0x8000)==0 ){
      bal &= 0x0FFF;

      if( bal==0 ){
        LED_BAL = LED_OFF;
      }else{
        LED_BAL = LED_ON;
      }

      vs.cfg.dcc8_1 = (unsigned char)(bal);
      vs.cfg.dcc12_9 = (unsigned char)(bal>>8);
    }
  }
  vs.cfg.mci4_1 = 0xF;
  vs.cfg.mci12_5 = 0xFF;
  vs.cfg.vuv = 0x00;
  vs.cfg.vov = 0x00;
  
  /* write config */
  (void)pin_low(cs);
  (void)ltc6802_wrcfg(da, &vs.cfg);
  (void)pin_high(cs);

  /* delay 20ms */
  delay_ms(20);

  /* start ad */
  (void)pin_low(cs);
  (void)ltc6802_stad(da, LTC6802_CMD_STCVAD);
//  (void)ltc6802_stad(da, LTC6802_CMD_STCVDC);
  (void)pin_high(cs);

  /* delay 20ms */
  delay_ms(20);
  
  /* read cvreg */
  (void)pin_low(cs);
  res = ltc6802_rdcv(da, &vs.cv);
  (void)pin_high(cs);
  if( res!=OK ) return;

  /* volt convert */
  {
    unsigned char i,j;
    unsigned short volt;
    for(i=0,j=0;j<6;j++){
      volt = (vs.cv.group[j].c0v11_8<<8) + vs.cv.group[j].c0v7_0; //  1.5mV/bit
      volt = volt + (volt+1)/2; //  1mV/bit
      vs_data[dev].volt[i] = volt;
      i++;
      volt = (vs.cv.group[j].c1v11_4<<4) + vs.cv.group[j].c1v3_0;
      volt = volt + (volt+1)/2; //  1mV/bit
      vs_data[dev].volt[i] = volt;
      i++;
    }
  }

  /* start tmpad */
  (void)pin_low(cs);
  (void)ltc6802_stad(da, LTC6802_CMD_STTMPAD);
  (void)pin_high(cs);

  /* delay 20ms */
  delay_ms(20);
  
  /* read tmpreg */
  (void)pin_low(cs);
  res = ltc6802_rdtmp(da, &vs.tmp);
  (void)pin_high(cs);
  if( res!=OK ) return;

  
  /* save temp */
  {
    unsigned short volt;

    volt = (vs.tmp.etmp1_11_8<<8) + vs.tmp.etmp1_7_0;
    vs_data[dev].vtemp1 = volt+(volt+1)/2;

    volt = (vs.tmp.etmp2_11_4<<4) + vs.tmp.etmp2_3_0;
    vs_data[dev].vtemp2 = volt+(volt+1)/2;

    volt = (vs.tmp.itmp_11_8<<8) + vs.tmp.itmp_7_0;
    volt *= 10; //  15mV/bit
    volt = volt+(volt+1)/2; //  10mV/bit
    volt = (volt+4)/8-2730+400; //  -40.0C~
    vs_data[dev].itemp = volt;
  }
}

void vs_init(void)
{

  unsigned char i, j;

  for(i=0;i<LTC6802_MAX;i++){
    vs_data[i].balance = 0x8000;
    for(j=0;j<12;j++) vs_data[i].volt[j] = 0x8000;
    vs_data[i].itemp = 0x8000;
    vs_data[i].vtemp1 = 0x8000;
    vs_data[i].vtemp2 = 0x8000;
  }

  (void)ltc6802_init();
  for(i=0;i<gv_sSC.sVS.bLtc6802Count;i++){
    (void)pin_high(gv_sSC.sVS.saLtc6802[i].eLtc6802EnaN);
  }

}

void vs_proc(void)
{
  unsigned char i;
  for(i=0;i<gv_sSC.sVS.bLtc6802Count;i++){
    vs_clear(i);
    vs_scan(i);
  }
}

void vs_reset(void)
{
  unsigned char i;
  for(i=0;i<gv_sSC.sVS.bLtc6802Count;i++){
    (void)pin_high(gv_sSC.sVS.saLtc6802[i].eLtc6802EnaN);
  }
  (void)ltc6802_reset();
}
