/**
  ******************************************************************************
  * proj    firmware startup project
  * file    WX.c
  * ver     1.0
  * brief   This is a WX file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _WX_C_  0x72
#define FILE_No _WX_C_

/* Includes ------------------------------------------------------------------*/
#include  "wx.h"  /* mandotary */
#include  "mcu.h"
#include  "vs.h"
#include  "ts.h"
#include  "bal.h"
#include  "cal.h"
#include  "sc.h"
#include  "pi.h"

/* Private Macros & Defines --------------------------------------------------*/

#ifndef   TUNE_WX
#define   TUNE_WX   0
#endif

#if TUNE_WX
#define WX_PRINTF PRINTF
#else
#define WX_PRINTF empty_printf
#endif

/* Private typedefs ----------------------------------------------------------*/

typedef union
{
  volatile unsigned char all;
  struct
  {
    volatile unsigned heater:1;
    volatile unsigned fan:1;
    volatile unsigned bal:1;
    volatile unsigned sync:1;
    volatile unsigned sleep:1;
    volatile unsigned debug:1;
    volatile unsigned :2;
  }
  bit;
}
wx_ctrl_t;

typedef union
{
  volatile unsigned char  all;
  struct
  {
    volatile unsigned :4;
    volatile unsigned heat:1;
    volatile unsigned ext_bal:1;
    volatile unsigned fan:1;
    volatile unsigned int_bal:1;
  }
  bit;
}
wx_status_t;

typedef union
{
  volatile unsigned char  all;
  struct
  {
    volatile unsigned :2;
    volatile unsigned heat:1;
    volatile unsigned fan:1;
    volatile unsigned ts:1;
    volatile unsigned harness:1;
    volatile unsigned bal:1;
    volatile unsigned comm:1;
  }
  bit;
}
wx_errors_t;

typedef struct {

  //  can rx
  unsigned char msg100_lock;
  unsigned char msg100_age;
  can_frame_t msg100;

  //  control
  wx_ctrl_t ctrl;
  wx_status_t status;
  wx_errors_t errors;
  unsigned long balance;

  //  output
  can_frame_t frame;

}wx_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

wx_t  wx;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/
static unsigned short Firdata[6];
static unsigned char F_index;
/* Global variables ----------------------------------------------------------*/
//extern unsigned char fanStatusSY;

/* Functions -----------------------------------------------------------------*/

/**
  * bref    WX function
  * param   none
  * retval  none
  */

//void voltSelect(unsigned short cellVolt, unsigned char cellNum);

void wx_init(void)
{

  /* runtime */
  wx.ctrl.all = 0;
  wx.balance = 0x000000;  /* cell 24,23,22,21,20,...3,2,1 */
  
  //fanStatus = 0;    //fan status inital
  gv_sSC.sWX.bFanStatusLow = 0;
  gv_sSC.sWX.bFanStatusHigh = 0; 
  
  //  sync
  dec_tick = 0;
  
  /* lower level */
  (void)can_init();
}

void wx_reset(void)
{
  /* lower level */
  (void)can_reset();
}

void wx_crx_save(can_frame_t * f)
{

  /* msg100 */
  /*if( f->id==0x100 && wx.msg100_lock==0 ){
    wx.msg100_age = 0;
    wx.msg100 = *f;
  } */
  if( f->id==0x013){
     Firdata[0]  = f->data[0]<<8;
     Firdata[0] |= f->data[1];
     Firdata[1]  = f->data[2]<<8;
     Firdata[1] |= f->data[3];
     Firdata[2]  = f->data[4]<<8;
     Firdata[2] |= f->data[5];
     Firdata[3]  = f->data[6]<<8;
     Firdata[3] |= f->data[7];
  }
      
}



void wx_sync(void)
{

  if( wx.ctrl.bit.sync ){

    /* wait next msg100 or timeout */
    dec_tick = gv_sSC.sWX.bSyncCycle * gv_sSC.sWX.bSyncTmoMul;
    while( wx.msg100_age>0 && dec_tick>0 ) feed_watch_dog();

    /* timeout */
    if( dec_tick==0 ){
      /* clear sync */
      wx.ctrl.bit.sync = 0;
      /* set sync cycle */
      dec_tick = gv_sSC.sWX.bSyncCycle;
    }

  }else{

    /* wait cycle */
    while( dec_tick>0 ) feed_watch_dog();

    /* set sync cycle */
    dec_tick = gv_sSC.sWX.bSyncCycle;

  }

}

unsigned char wx_crx_proc(void)
{

  unsigned char exit = 0;

  /* lock msg in case access from ISR */
  wx.msg100_lock = 1;

  /* new msg */
  /*if( wx.msg100_age==0 ){
    unsigned char tgt;

    // match modnum 
    tgt = wx.msg100.data[0];
    if( tgt==0xff || tgt==gv_sSC.sWX.bModNum ){
      // control bits 
      wx.ctrl.all = wx.msg100.data[1];
      // bmu sleep 
      if( wx.ctrl.bit.sleep ){
        exit = 1;
        DBG_PRINTF("sleep from 0x100 cmd\r\n");
      }
      // balance 
      if( wx.ctrl.bit.bal ){
        (void)bal_ena_int(1);
      }else{
        (void)bal_ena_int(0);
      }
    }*/
    bal_ena_int(0);
    
   // return 0;
    
  //}

  /* age up */
//  if( wx.msg100_age<0xFF ) wx.msg100_age ++;

  /* unlock */
//  wx.msg100_lock = 0;

  return exit;

}

void wx_ctx(void)
{

static unsigned char cnt = 0;
    
#if TEMP
cnt++;
if(cnt < 5) return; 
cnt = 0;
{
  unsigned char b;
  unsigned short h;
  //  id, dlc
  wx.frame.id = 0x006 + (gv_sSC.sWX.bModNum<<4);
  wx.frame.dlc = gv_sSC.sWX.bTempCount;
  //  T0~2
  for(b=0;b<gv_sSC.sWX.bTempCount;b++)      //3 is the temp resistor number for SY project 
  {
    h = (cal.temp[b] + 5) / 10;
    if( h>0xFF ) h = 0xFF;
    //if((gv_sSC.sVS.bLtc6802Count == 1) && (b >= gv_sSC.sWX.bTempCount)) h = 0xFF;
    wx.frame.data[b] = (unsigned char)h; 
  }
  /* xmit */
  (void)can_xmit(&wx.frame);
  delay_ms(1);   
}  

#endif


}

void Fir_pro(void){
 unsigned char i;
 for(i=0;i<6;i++){
 if(Firdata[i]>=1000) F_index=1;
 }
 if(F_index){
 wx.frame.id = 0x011 ;
 wx.frame.dlc = 1;
 wx.frame.data[0] = (unsigned char)11; 
 (void)can_xmit(&wx.frame);
 delay_ms(1);   
 }
}

