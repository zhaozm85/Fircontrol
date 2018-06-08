/**
  ******************************************************************************
  * proj    firmware startup project
  * file    CAL.c
  * ver     1.0
  * brief   This is a CAL file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _CAL_C_  0x78
#define FILE_No _CAL_C_

/* Includes ------------------------------------------------------------------*/
#include  "cal.h"  /* mandotary */
#include  "sc.h"
#include  "vs.h"
#include  "ts.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

cal_t cal;

/* Functions -----------------------------------------------------------------*/

/**
  * bref    CAL function
  * param   none
  * retval  none
  */

void cal_init(void)
{
}

static void cal_clear(void)
{
  unsigned char i;
  
  //  volt
  for(i=0;i<12*LTC6802_MAX;i++) cal.volt[i] = 0x8000;
  cal.vmax = 0x8000;
  cal.vMaxLow = 0x8000;
  cal.vMaxHigh = 0x8000;
  cal.vmin = 0x8000;
  cal.vMinLow = 0x8000;
  cal.vMinHigh = 0x8000;
  cal.vsum = 0x80000000;
  cal.vSumLow = 0x80000000;
  cal.vSumHigh = 0x80000000;
  cal.vave = 0x8000;        
  cal.vAveLow = 0x8000;
  cal.vAveHigh = 0x8000;
  cal.ivmax = 0x80;
  cal.ivMaxLow = 0x80;
  cal.ivMaxHigh = 0x80;
  cal.ivmin = 0x80;
  cal.ivMinLow = 0x80;
  cal.ivMinHigh = 0x80;
  
  //  temp
  for(i=0;i<24;i++) cal.temp[i] = 0x8000;
  cal.tmax = 0x8000;
  cal.tMaxLow = 0x8000;
  cal.tMaxHigh = 0x8000;
  cal.tmin = 0x8000;
  cal.tMinLow = 0x8000;
  cal.tMinHigh = 0x8000;
  cal.tsum = 0x80000000;
  cal.tSumLow = 0x80000000;
  cal.tSumHigh = 0x80000000;
  cal.tave = 0x8000;
  cal.tAveLow = 0x8000;
  cal.tAveHigh = 0x8000;
  cal.itmax = 0x80;
  cal.itMaxLow = 0x80;
  cal.itMaxHigh = 0x80;
  cal.itmin = 0x80;
  cal.itMinLow = 0x80;
  cal.itMinHigh = 0x80;

  //  other temps
  cal.bal_temp = 0x8000;
  cal.main_temp = 0x8000;
  
}

static void cal_volt(void)
{
  unsigned char i;
  unsigned char cnt;

  //  get voltages
  for(i=0;i<gv_sSC.sCal.bVoltCount;i++){
    unsigned char si;
    unsigned char ii;
    signed short  volt;

    ii = gv_sSC.sCal.baVoltMap[i];
    si = ii>>4;
    ii &= 0x0f;

    if( ii>=gv_sSC.sCal.bVoltCount ) continue;
    
    if( si<LTC6802_MAX ){
      volt = vs_data[si].volt[ii];
    }else{
      volt = gv_sSC.sCal.haVoltRef[ii];
    }

    volt += gv_sSC.sCal.sbaVoltAdj[i];
    
    if( volt<0 ) volt = 0;
    
    cal.volt[i] = volt;

  }

  //  vmax, vmin, vsum
  cnt = 0;
  //if(gv_sSC.sVs.bLtc6802Count == 2)
  for(i=0;i<gv_sSC.sCal.bVoltCount;i++){
    unsigned short volt;
    volt = cal.volt[i];

    //  do if valid
    if( (volt&0x8000)==0 ){

      //  max
      if( (cal.vmax&0x8000) || cal.vmax<volt ){
        cal.vmax = volt;
        cal.ivmax = i;
        if(i < gv_sSC.sCal.bCellCount) 
        {
          cal.vMaxLow = volt;
          cal.ivMaxLow = i;
        } 
        else 
        {
          cal.vMaxHigh = volt;
          cal.ivMaxHigh = i;
        }
      }

      //  min
      if( (cal.vmin&0x8000) || cal.vmin>volt ){
        cal.vmin = volt;
        cal.ivmin = i;
        if(i < gv_sSC.sCal.bCellCount) 
        {
          cal.vMinLow = volt;
          cal.ivMinLow = i;
        } 
        else 
        {
          cal.vMinHigh = volt;
          cal.ivMinHigh = i;
        }
      }

      //  sum
      if( cal.vsum&0x80000000 ){
        cal.vsum = volt;
        cal.vSumLow = volt;
        cal.vSumHigh = volt;
      }else{
        cal.vsum += volt;
        if(i < gv_sSC.sCal.bCellCount) 
        {
          cal.vSumLow += volt;
        } 
        else
        {
          cal.vSumHigh += volt;
        }
      }
      
      //  count up
      cnt++;

    }
    
  }

  //  average voltage
  if( (cal.vsum&0x80000000)==0 && cnt>0 ){
    unsigned long tmp;
    tmp = (cnt+1)/2;
    tmp += cal.vsum;
    tmp /= cnt;
    if( tmp>0x7FFF ) tmp=0x7FFF;
    cal.vave = (unsigned short)tmp;
  }

  /* bal temp */
  for( i=0;i<gv_sSC.sVS.bLtc6802Count;i++){
    unsigned short ti;
    ti = vs_data[i].itemp;
    if( (ti&0x8000)==0 ){
      if( (cal.bal_temp&0x8000) || cal.bal_temp<ti ){        
        cal.bal_temp = ti;
      }
    }
  }

}

static void cal_temp(void)
{
  unsigned char i;
  unsigned char cnt;

  /* get temps */
  for(i=0;i<gv_sSC.sCal.bTempCount;i++){
    unsigned char si;
    unsigned char ii;
    
    ii = gv_sSC.sCal.baTempMap[i];
    si = ii>>5;
    ii &= 0x1F;
    
    if( ii>=24 ) continue;
    
    if( si==0 ){
      cal.temp[i] = ts_data.temp[ii];  //  from original temp
    }else if( si==1 ){
      ii = gv_sSC.sTS.baList1[ii];
      if( ii<24 ) cal.temp[i] = ts_data.temp[ii]; //  from list 1
    }else if( si==2 ){
      ii = gv_sSC.sTS.baList2[ii];
      if( ii<24 ) cal.temp[i] = ts_data.temp[ii]; //  from list 2
    }else{
      cal.temp[i] = gv_sSC.sCal.haTempRef[ii];    //  from ref temp
    }
  
  }
  
  /* tsum, tmax, tmin */
  cnt = 0;
  for(i=0;i<gv_sSC.sCal.bTempCount;i++){
    unsigned short temp;
    temp = cal.temp[i];
    
    //  do if valid
    if( (temp&0x8000)==0 ){
    
      //  tmax
      if(i < gv_sSC.sCal.bResistorCount) 
      {
        if( (cal.tmax&0x8000) || (cal.tmax<temp))
        {
          cal.tmax = temp;
          cal.itmax = i;
          cal.tMaxLow = temp;
          cal.itMaxLow = i;
        }  
      } 
     /* else 
      {
        if( (cal.tmax&0x8000) || (cal.tmax<temp) || (cal.tMaxHigh&0x8000) || (cal.tMaxHigh<temp))
          {
            cal.tmax = temp;
            cal.itmax = i;
            cal.tMaxHigh = temp;
            cal.itMaxHigh = i;
          }  
      }*/
      
      //  tmin
       if(i < gv_sSC.sCal.bResistorCount) 
      {
        if( (cal.tmin&0x8000) || (cal.tmin>temp))
        {
          cal.tmin = temp;
          cal.itmin = i;
          cal.tMinLow = temp;
          cal.itMinLow = i;
        }  
      } 
     /* else 
      {
        if( (cal.tmin&0x8000) || (cal.tmin>temp) || (cal.tMinHigh&0x8000) || (cal.tMinHigh>temp))
          {
            cal.tmin = temp;
            cal.itmin = i;
            cal.tMinHigh = temp;
            cal.itMinHigh = i;
          }  
      } */
     
      //  tsum
      if( cal.tsum&0x80000000 ){
        cal.tsum = temp;
        cal.tSumLow = temp;
        cal.tSumHigh = temp;
      }else{
        cal.tsum += temp;
        if(i < gv_sSC.sCal.bResistorCount) 
        {
          cal.tSumLow += temp;
        } 
        else
        {
          cal.tSumHigh += temp;
        }
      }
      
      //  count up
      cnt++;

    }
  }
  
  /* tave */
  if( (cal.tsum&0x80000000)==0 && cnt>0 ){
    unsigned long tmp;
    tmp = (cnt+1)/2;
    tmp += cal.tsum;
    tmp /= cnt;
    if( tmp>0x7FFF ) tmp=0x7FFF;
    cal.tave = (unsigned short)tmp;
  }
  
  /* sort */
  if( cnt>0 ){
    for(i=0;i<cnt;i++) cal.temp_sorted[i] = cal.temp[i];
    if( cnt>1 ){
      for(i=0;i<cnt-1;i++){
        unsigned char j;
        for(j=i;j<cnt;j++){
          unsigned short ti, tj;

          ti = cal.temp_sorted[i];
          tj = cal.temp_sorted[j];
          
          if( (ti&0x8000)!=0 && (tj&0x8000)==0 ){
            cal.temp_sorted[i] = tj;
            cal.temp_sorted[j] = ti;
            continue;
          }

          if( (ti&0x8000)==0 && (tj&0x8000)==0 ){
            if( ti<tj ){
              cal.temp_sorted[i] = tj;
              cal.temp_sorted[j] = ti;
              continue;
            }
          }
        }
      }
    }
  }

  //  main temp
  if( (cal.bal_temp&0x8000)==0 ){
    cal.main_temp = cal.bal_temp;
    if( (ts_data.mcu_temp&0x8000)==0 && cal.main_temp<ts_data.mcu_temp){
      cal.main_temp = ts_data.mcu_temp;
    }
  }else{
    cal.main_temp = ts_data.mcu_temp;
  }
}

void cal_proc(void)
{
  cal_clear();
  cal_volt();
  cal_temp();
}

void cal_reset(void)
{
}

