/**
  ******************************************************************************
  * proj    BMU_HM
  * file    sc.h
  * ver     1.0
  * brief   系统配置
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
/* 防止头文件被重复引用 ------------------------------------------------------*/

#ifndef _SC_H_
#define _SC_H_

/* Includes ------------------------------------------------------------------*/
/* 本头文件涉及的头文件 ------------------------------------------------------*/

#include  "mcu.h"
#include  "bmu.h"

/* Global Macros & Defines ---------------------------------------------------*/
/* 全局宏定义 ----------------------------------------------------------------*/

/* Global typedefs -----------------------------------------------------------*/
/* 全局类型定义 --------------------------------------------------------------*/

typedef struct
{
  /* 信息头部 */
  struct{
    volatile unsigned short hLength;  /* 结构体的长度 */
    volatile unsigned short hMagicCode;   /* 校验用 */
    volatile unsigned char  bCSEven;  /* 偶部分的校验和 */
    volatile unsigned char  bCSOdd;   /* 奇部分的校验和 */
    /* 创建时间和更新时间记以2000.1.1 0:00:00开始经过的秒数 */
    volatile unsigned long  wCreateTime;  /* 创建时间 */
    volatile unsigned long  wUpdateTime;  /* 更新时间 */
    volatile unsigned short hUpdateCount; /* 更新次数 */
  }sHead;

  /* 基本配置 */
  struct{
    volatile gt_eCanBaudrate  eCanBaudrate;
    volatile gt_eBmuMode      eDefaultMode;
  }sBMU;

  struct{
    volatile unsigned char    bLtc6802Count;
    struct{
      volatile unsigned char  bDeviceAddress;
      volatile gt_ePinNumber  eLtc6802EnaN;
      volatile gt_ePinNumber  eLtc6802SpiCsN;
    }saLtc6802[LTC6802_MAX];
  }sVS;
  
  struct{
    volatile unsigned short hAdcWaitMSecs;
    volatile unsigned short hAdcRepeatCount;
    volatile unsigned char  baSensorType[24];
    volatile unsigned char  bCount1;
    volatile unsigned char  baList1[24];
    volatile unsigned char  bCount2;
    volatile unsigned char  baList2[24];
    volatile unsigned short hVtemp25;
    volatile unsigned short hMHot;
    volatile unsigned short hMCold;
  }sTS;
  
  struct{
    volatile unsigned short haVoltRef[12];
    volatile unsigned char  bVoltCount;
    volatile unsigned char  bCellCount;
    volatile unsigned char  baVoltMap[12*LTC6802_MAX];
    volatile signed char    sbaVoltAdj[12*LTC6802_MAX];
    volatile unsigned short haTempRef[24];
    volatile unsigned char  bTempCount;
    volatile unsigned char  bResistorCount;
    volatile unsigned char  baTempMap[24];
  }sCal;

  struct{
    volatile unsigned char  bLowEnabled;
    volatile unsigned char  bLowRoundCycles;
    volatile unsigned char  bLowBalanceCycles;
    volatile unsigned short hLowTempCool;
    volatile unsigned short hLowTempHot;
    volatile unsigned char  bExtEnabled;
    volatile unsigned char  bIntEnabled;
    volatile unsigned short hIntRoundCycles;
    volatile unsigned short hIntBalanceCycles;
    volatile unsigned short hIntVDiffBad;
  }sBal;

  struct{
    volatile unsigned short hGroupNum;
    volatile unsigned char  bModNum;
    volatile unsigned char  bSyncCycle;
    volatile unsigned char  bSyncTmoMul;
    volatile unsigned char  bVoltCount;
    volatile unsigned char  bCellCount;
    volatile unsigned char  bTempCount;
    volatile unsigned char  bResistorCount;
    volatile unsigned char  bFanStatusLow;
    volatile unsigned char  bFanStatusHigh;
    volatile unsigned char  baTempMap[8];  //  0x00~, 0x20~, 0x40~, 0xE0~   
  }sWX;

}
gt_sSC;

/* Global consts -------------------------------------------------------------*/
/* 全局常量声明 --------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
/* 全局变量声明 --------------------------------------------------------------*/

extern gt_sSC gv_sSC;

/* Global function prototypes ------------------------------------------------*/
/* 全局函数声明 --------------------------------------------------------------*/

extern int gf_nSCInit(void);
extern int gf_nSCLoad(void);
extern int gf_nSCSave(void);

/* File Id & Re-include control ----------------------------------------------*/
/* 防止头文件被重复引用 ------------------------------------------------------*/

#endif  /* _SC_H_ */

