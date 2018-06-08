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
#define _BAL_C_  0x76
#define FILE_No _BAL_C_

/* Includes ------------------------------------------------------------------*/
#include  "bal.h"  /* mandotary */
#include  "sc.h"
#include  "vs.h"
#include  "mcu.h"
#include  "can.h"
#include  "cal.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  //  low level control
  unsigned char   low_cnt;  //  cycle count
  unsigned long   low_req;  //  final balance;
  unsigned long   low_stt;  //  balance status
  unsigned char   low_hot;      //  hot status
  //  external
  unsigned char   ext_ena;
  unsigned long   ext_bal;
  //  internal
  unsigned char   int_ena;
  unsigned long   int_bal;
  unsigned short  int_cnt;
}
bal_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

bal_t bal;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    template function
  * param   none
  * retval  none
  */

void  bal_init(void)
{
  bal.ext_bal = 0;
  bal.ext_ena = 0;
  bal.int_bal = 0;
  bal.int_ena = 0;
  bal.int_cnt = 0;
  bal.low_req = 0;
  bal.low_stt = 0;
  bal.low_cnt = 0;
  bal.low_hot = 0;
}

void  bal_reset(void)
{
}

static void bal_proc_low(void){

  unsigned long fb;

  //  enable control
  if( gv_sSC.sBal.bLowEnabled==0 ) return;

  //  temp check
  if( cal.bal_temp < gv_sSC.sBal.hLowTempCool ){
    bal.low_hot = 0;
  }else if( cal.bal_temp > gv_sSC.sBal.hLowTempHot ){
    bal.low_hot = 1;
  }
  
  //  balance status
  if( bal.low_hot ){
    bal.low_stt = 0;
  }else{
    bal.low_stt = bal.low_req;
  }

  //  duty control
  if( bal.low_cnt < gv_sSC.sBal.bLowBalanceCycles ){
    fb = bal.low_stt;
  }else{
    fb = 0;
  }
  bal.low_cnt ++;
  if( bal.low_cnt >= gv_sSC.sBal.bLowRoundCycles ){
    bal.low_cnt = 0;
  }

  //  set to 6802
#if LTC6802_MAX>2
#error  balance support max 2 ltc6802s
#else
  {
    unsigned char i;
    for(i=0;i<LTC6802_MAX;i++){
      unsigned short b;
      b = (unsigned short)(fb&0xFFF);
      vs_data[i].balance = b;
      fb >>= 12;
    }
  }
#endif  

}

static unsigned long bal_get_ibal(void){
  unsigned char   i;
  unsigned long   mask;
  unsigned long   ibal;
  unsigned short  vmin;

  ibal = 0;  
  mask = 1;
  vmin = cal.vmin;
  DBG_PRINTF("%u: ",vmin);

  for(i=0;i<gv_sSC.sCal.bVoltCount;i++){
    unsigned short  volt;
    unsigned short  diff;
    
    volt = cal.volt[i];
    DBG_PRINTF("%u", volt);
    if( volt>vmin ){
      diff = volt-vmin;
      if( diff > gv_sSC.sBal.hIntVDiffBad ){
        ibal |= mask;
        DBG_PRINTF("-, ");
      }else{
        DBG_PRINTF("_, ");
      }
    }else{
      DBG_PRINTF("_, ");
    }
    
    /* next cell */
    mask <<= 1;
    
  }

  DBG_PRINTF(">> %lxh\r\n", ibal);
  return ibal;
}

static void bal_proc_ext(void)
{
  //  enable control
  if( bal.ext_ena && gv_sSC.sBal.bExtEnabled ){

    //  set to low
    bal.low_req = bal.ext_bal;
  }else{

    //  clear external balance
    bal.ext_bal = 0;
  }
}

static void bal_proc_int(void)
{
  //  if external balance is on
  if( bal.ext_ena && gv_sSC.sBal.bExtEnabled ){
  
    //  clear internal balance
    bal.int_bal = 0;
    return;
  }

  //  set internal balance
  if( bal.int_ena && gv_sSC.sBal.bIntEnabled )
  {

    //  cycle control
    if( bal.int_cnt==0 ){
      bal.int_bal = bal_get_ibal();
    }else if( bal.int_cnt==gv_sSC.sBal.hIntBalanceCycles ){
      bal.int_bal = 0;
    }
    bal.int_cnt ++;
    if( bal.int_cnt>=gv_sSC.sBal.hIntRoundCycles ) bal.int_cnt = 0;
  
    //  set to low
    bal.low_req = bal.int_bal;
  }
  else 
  { 
    //  clear internal balance
    bal.int_bal = 0;
  }
}

void bal_proc(void)
{

  /* low level */
  bal_proc_low();
  
  /* external balance */
  bal_proc_ext();

  /* internal balance */  
  bal_proc_int();

}

void bal_set_ext(unsigned long ebal)
{
  bal.ext_bal = ebal;
}

void bal_ena_ext(unsigned char ena)
{
  bal.ext_ena = ena;
}

unsigned char bal_get_ext(void)
{
  if( bal.ext_ena && gv_sSC.sBal.bExtEnabled ) return 1;
  return 0;
}

void bal_ena_int(unsigned char ena) 
{
  bal.int_ena = ena;
}

unsigned char bal_get_int(void)
{
  if( bal.ext_ena && gv_sSC.sBal.bExtEnabled ) return 0;
  if( bal.int_ena && gv_sSC.sBal.bIntEnabled ) return 1;
  return 0;
}

