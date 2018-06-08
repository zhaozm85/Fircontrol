/**
  ******************************************************************************
  * proj    firmware startup project
  * file    TS.c
  * ver     1.0
  * brief   This is a TS file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _TS_C_  0x75
#define FILE_No _TS_C_

/* Includes ------------------------------------------------------------------*/
#include  "ts.h"  /* mandotary */
#include  "mcu.h"
#include  "sc.h"
#include  "ntc.h"
#include  "can.h"

/* Private Macros & Defines --------------------------------------------------*/

#define   ADP_MCU_TEMP  26

#define   R0  9100  //  9.1Kohm

#ifndef   TUNE_TS
#define   TUNE_TS   0
#endif

#if TUNE_TS
#define TS_PRINTF PRINTF
#else
#define TS_PRINTF empty_printf
#endif

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  unsigned char count1;
}
ts_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

ts_t  ts;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

ts_data_t ts_data;

/* Functions -----------------------------------------------------------------*/

/**
  * bref    TS function
  * param   none
  * retval  none
  */

static unsigned short ts_adc_to_temp(unsigned short adc, unsigned char type)
{

  long tmp;

  //  adjust adc
  if( adc>0xFFE ) adc = 0xFFE;

  //  adc to res
  /*  R0   0xFFF - ADC
      -- = -----------
      Rt        ADC    */
  tmp = R0;
  tmp *= adc;
  tmp /= (0xFFF-adc);
//  if( tmp>0xFFFF ) tmp = 0xFFFF;
  TS_PRINTF("Rt: %ld; ", tmp);
  
  //  res to temp
  tmp = ntc_res_to_temp(tmp,type);
  TS_PRINTF("-> %ld,", tmp);
  
  if( tmp<-400 ) tmp = -400;
  tmp += 400;

  return  (unsigned short)tmp;

}

static int  ts_adc_to_mcu_temp(unsigned short adc)
{

  volatile signed long Vtemp, Vtemp25;
  volatile signed long tmp;

  Vtemp = 12207; //  1000*5000*10/4096;
  Vtemp *= adc;   //  0.1uV/bit  
  Vtemp25 = 1000*10;
  Vtemp25 *= gv_sSC.sTS.hVtemp25; //  0.1uV/bit
  TS_PRINTF("(%ld-%ld), ", Vtemp, Vtemp25);
  tmp = Vtemp-Vtemp25;
  TS_PRINTF("%ld, ", tmp);
  if( tmp>0 ) tmp /= gv_sSC.sTS.hMCold;   //  0.1uV / (1uV/C) = 0.1C
  else tmp/=gv_sSC.sTS.hMHot;
  TS_PRINTF("%ld, ", tmp);
  tmp = 25*10 - tmp;  //  0.1C, 0.0C~
  TS_PRINTF("%ld, ", tmp);
  if( tmp<-400 ) tmp = -400;  //  no less than -40.0C
  tmp += 400; //  -40.0C~
  
  return  (int)tmp;

}

static int  ts_sense(unsigned char adp)
{

  int res;
  can_frame_t msg;
  
  if( adp>=24 && adp!=ADP_MCU_TEMP ){
    PERRNO(ERRNO_INVALID_PARAMETERS,adp);
    return  ERR;
  }
  TS_PRINTF("ts: ");

//  DBG_PRINTF("adc\n");
  res = adc_conv(adp, gv_sSC.sTS.hAdcWaitMSecs, gv_sSC.sTS.hAdcRepeatCount);
  if( res<0 ) return  res;
  TS_PRINTF("%d, ", res);
  //msg.data[0] = (unsigned char)(res>>8);
  //msg.data[1] = (unsigned char)res;

//  DBG_PRINTF("to temp\n");
  if( adp==ADP_MCU_TEMP ){
    res = ts_adc_to_mcu_temp(res);
  }else{    
    res = ts_adc_to_temp(res, gv_sSC.sTS.baSensorType[adp]);
  }
  if( res<0 ) return  res;
  TS_PRINTF("%d\r\n", res);
  //msg.data[2] = (unsigned char)(res>>8);
  //msg.data[3] = (unsigned char)res;

  if( adp==ADP_MCU_TEMP ) ts_data.mcu_temp = res;
  else  ts_data.temp[adp] = res;
  
  /*msg.id = 0x700+adp;
  msg.dlc = 4;
  if( adp!=ADP_MCU_TEMP ) (void)can_xmit(&msg); */

  return res;

}

void ts_init(void)
{

  unsigned char i;

  ts.count1 = 0;
  for(i=0;i<24;i++) ts_data.temp[i] = 0x8000;
  ts_data.mcu_temp = 0x8000;

  (void)adc_init();

}

void ts_proc(void)
{

  unsigned char i;

  //  list 1
//  DBG_PRINTF("lst1\n");
  if( gv_sSC.sTS.bCount1>0 ){
    (void)ts_sense( gv_sSC.sTS.baList1[ts.count1] );
    ts.count1 ++;
    if( ts.count1>=gv_sSC.sTS.bCount1 ) ts.count1 = 0;
  }
  
  //  list 2
//  DBG_PRINTF("lst2\n");
  if( gv_sSC.sTS.bCount2>0){    
    for(i=0;i<gv_sSC.sTS.bCount2;i++){
      (void)ts_sense( gv_sSC.sTS.baList2[i] );
    }
  }
  
  //  mcu temp
//  DBG_PRINTF("mcu\n");
  //(void)ts_sense( ADP_MCU_TEMP );
  
}

void ts_reset(void)
{
  (void)adc_reset();
}

