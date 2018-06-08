/**
  ******************************************************************************
  * proj    firmware startup project
  * file    BMU.c
  * ver     1.0
  * brief   This is a BMU file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _BMU_C_ 0x71
#define FILE_No _BMU_C_

/* Includes ------------------------------------------------------------------*/
#include  "bmu.h"  /* mandotary */
#include  "can.h"
#include  "wx.h"
#include  "mcu.h"
#include  "vs.h"
#include  "ltc6802.h"
#include  "ts.h"
#include  "bal.h"
#include  "cal.h"
#include  "pi.h"
#include  "sc.h"
#include  "mng.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  gt_eBmuMode eMode;
}
lt_sBmu;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

lt_sBmu lv_sBmu;
//unsigned char fanStatus;

/* Private function prototypes -----------------------------------------------*/
unsigned char lf_fanStatusLow(void);
unsigned char lf_fanStatusHigh(void);

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

static int lf_nBmuLoad(void)
{
  int res;
  DBG_PRINTF("lf_nBmuLoad()\r\n");
  
  /* 加载产品信息 */
  res = gf_nPIInit(); if( res!=OK ) return res;  
  res = gf_nPILoad(); if( res!=OK ) return res;
  
  /* 加载系统配置 */
  res = gf_nSCInit(); if( res!=OK ) return res;  
  res = gf_nSCLoad(); if( res!=OK ) return res;
  
  /* 完成 */
  DBG_PRINTF(" - OK: lf_nBmuLoad\r\n");
  return  OK;  
}

static gt_eBmuMode lf_eBmuRunConfig(void)
{
  unsigned char exit = 0;
  DBG_PRINTF("lf_eBmuRunConfig()\r\n");
  
  /* 开启各个模块 */
  mng_init();
  
  /* 开始运行 */
  while(exit==0){

    /* 喂狗 */
    feed_watch_dog();
  
    /* 处理CAN接收 */
    exit = mng_crx_proc();

  }

  /* 关闭各个模块 */
  mng_reset();

  DBG_PRINTF(" - End: lf_eBmuRunConfig\r\n");
  /* 下一个模式 */
  return  BMU_MODE_NORMAL;
}

static gt_eBmuMode lf_eBmuRunNormal(void)
{
  unsigned char exit = 0;
  DBG_PRINTF("lf_eBmuRunNormal()\r\n");
  
  /* 开启各功能模块 */
  wx_init();
  //vs_init();
 // ts_init();
  //cal_init();
  //bal_init();

  /* 运行各功能模块 */
  while(exit==0){

    /* 喂狗 */  
    feed_watch_dog();

    /* 同步 */
//    DBG_PRINTF("sync\n");
   wx_sync();
 (void)mscan_rxcb(&mscan_rxframe);
    /* CAN接收 */
//    DBG_PRINTF("crx\n");
   // exit = wx_crx_proc();
    
    /* 电压采集 */
//    DBG_PRINTF("vs\n");
   // vs_proc();

    /* 温度采集 */
//    DBG_PRINTF("ts\n");
  //  ts_proc();
    
    /* 计算统计 */
//    DBG_PRINTF("cal\n");
  //  cal_proc();

    /* 均衡控制 */
//    DBG_PRINTF("bal\n");
 //   bal_proc();
    
    //fan status
    if(gv_sSC.sVS.bLtc6802Count == 2)
    {
      gv_sSC.sWX.bFanStatusLow = lf_fanStatusLow();
     // gv_sSC.sWX.bFanStatusLow = 0;
      gv_sSC.sWX.bFanStatusHigh = lf_fanStatusHigh();
    } 
    else
    {
      gv_sSC.sWX.bFanStatusLow = lf_fanStatusLow();
      gv_sSC.sWX.bFanStatusHigh = lf_fanStatusHigh();
    } 
    
    /* 热控制 */
        
    /* CAN发送 */
//    DBG_PRINTF("ctx\n\r");
    wx_ctx();
    Fir_pro();
  }
  
  /* 关闭各个功能模块 */
  bal_reset();
  cal_reset();
  ts_reset();
  vs_reset();
  wx_reset();

  DBG_PRINTF(" - End: lf_eBmuRunNormal\r\n");
  /* 下一个工作模式 */  
  return  BMU_MODE_CONFIG;
}

// 检测高端6802是否存在
static void lf_vBmuTestB(void)
{
  gt_ePinNumber en = gv_sSC.sVS.saLtc6802[1].eLtc6802EnaN;
  gt_ePinNumber cs = gv_sSC.sVS.saLtc6802[1].eLtc6802SpiCsN;
  unsigned char da = gv_sSC.sVS.saLtc6802[1].bDeviceAddress;
  int res;
  static ltc6802_cfgreg_t cfg;

  (void)ltc6802_init();
  (void)pin_high(en); // 光偶打开
  delay_ms(20);    /* delay 20ms */

  (void)pin_low(cs);  // cs选中
  res = ltc6802_rdcfg(da,&cfg); // 读取配置寄存器
  (void)pin_high(cs); // cs释放  

  (void)pin_low(en);  // 光偶关闭
  delay_ms(20);    /* delay 20ms */
  (void)ltc6802_reset();  
  
  if( res==OK ) return; // 存在
  
  // 更改配置为12串
  gv_sSC.sVS.bLtc6802Count = 1; // 测量时只检测一个6802
 // gv_sSC.sCal.bVoltCount = 8; // 统计时只计算低端10个电压
  gv_sSC.sCal.bVoltCount = 10; // 统计时只计算低端10个电压
  gv_sSC.sCal.bTempCount = 3;     //the cell number is 3
 // gv_sSC.sWX.bVoltCount = 8;   // 输出时只输出10个电压
  gv_sSC.sWX.bVoltCount = 10;   // 输出时只输出10个电压
  gv_sSC.sWX.bTempCount = 3;      //the temp number is 3 
  gv_sSC.sTS.bCount2 = 0;         //single sample disable
  gv_sSC.sTS.bCount2 = 3;         //mutile sample number is 3 

}

//SY project cell number initial setup
static void lf_SYCellNumInit(void) 
{
  if(gv_sSC.sVS.bLtc6802Count == 3) 
  {
    gv_sSC.sVS.bLtc6802Count = 2;   //the default LTC6802 number is 2
    gv_sSC.sCal.bVoltCount = 20;    //the default cell number is 20
    gv_sSC.sCal.bTempCount = 3;     //the default cell number is 6
    gv_sSC.sWX.bVoltCount = 20;     //the default cell number is 20
    gv_sSC.sWX.bTempCount = 3;      //the default temp number is 6
  //  gv_sSC.sCal.bCellCount = 8;    //the default cell number of single Ltc6802
    gv_sSC.sCal.bCellCount = 10;    //the default cell number of single Ltc6802
    gv_sSC.sCal.bResistorCount = 3; //the default temp number of single Ltc6802
  //  gv_sSC.sWX.bCellCount = 8;     //the default cell number of single Ltc6802
    gv_sSC.sWX.bCellCount = 10;     //the default cell number of single Ltc6802
    gv_sSC.sWX.bResistorCount = 3;  //the default temp number of single Ltc6802
    gv_sSC.sTS.bCount2 = 0;         //single sample disable
    gv_sSC.sTS.bCount2 = 3;         //mutile sample number is 6
    
    lf_vBmuTestB();
  }
  
}

/**
  * bref    BMU function
  * param   none
  * retval  none
  */

void gf_vBmuMain(void)
{
  int res;
  unsigned int delayBMU = 0;
  DBG_PRINTF("bmu_main()\r\n");
  
  /* 加载配置 */
  res = lf_nBmuLoad();
  //SY project configure initialization
 // lf_SYCellNumInit();
  
  /* 初始模式 */   
  lv_sBmu.eMode = BMU_MODE_NONE;
  if( res==OK ) lv_sBmu.eMode = gv_sSC.sBMU.eDefaultMode;
  if( lv_sBmu.eMode==BMU_MODE_NONE ) lv_sBmu.eMode = BMU_MODE_CONFIG;
  DBG_PRINTF(" - Start Mode: %d\r\n", lv_sBmu.eMode);
  
  delayBMU = (unsigned int)(gv_sSC.sWX.bModNum % 30) * 6; 
  delay_ms(delayBMU);
  
  /* 按模式开始运行 */
  for(;;){
    switch( lv_sBmu.eMode ){
      case  BMU_MODE_NORMAL:
        lv_sBmu.eMode = lf_eBmuRunNormal();
        break;
      default:
        lv_sBmu.eMode = lf_eBmuRunConfig();
        break;
    }
  }

}

void gf_vCanRxcb(can_frame_t * f)
{
  /* 按模式保存消息 */
  switch( lv_sBmu.eMode ){
    case  BMU_MODE_NORMAL:
      wx_crx_save(f);
      break;
    default:
      mng_crx_save(f);
      break;
  }
}


unsigned char lf_fanStatusLow(void) 
{
  unsigned char fanTempDiffLow;
  fanTempDiffLow = (cal.tmax + 5) / 10 - (cal.tmin + 5) / 10;    //Temp threshold difference
  //fanTempDiffHigh = (cal.tMaxHigh + 5) / 10 - (cal.tMinHigh + 5) / 10;
  
  if((fanTempDiffLow >= FANSTARTTEMP) || (((cal.tmax + 5) / 10) >= FANMINSTARTTEMP)) 
  {
    //pin_low(PN_FAN_ENA);
    if((pin_high(PN_FAN_ENA) == 1) && (pin_high(PN_HEAT_ENA) == 1)) return 1;
    
    else return 0;
  }
  else if((fanTempDiffLow <= FANSTOPTEMP) && (((cal.tmax + 5) / 10) <= FANMAXSTOPTEMP))
  {
    if((pin_low(PN_FAN_ENA) == 1) && (pin_low(PN_HEAT_ENA) == 1)) return 0;
    else return 0;
  }
}

unsigned char lf_fanStatusHigh(void) 
{
  unsigned char fanTempDiffHigh;
  //fanTempDiffLow = (cal.tMaxLow + 5) / 10 - (cal.tMinLow + 5) / 10;    //Temp threshold difference
  fanTempDiffHigh = (cal.tmax + 5) / 10 - (cal.tmin + 5) / 10;   //Temp threshold difference
  
  if((fanTempDiffHigh >= FANSTARTTEMP) || (((cal.tmax + 5) / 10) >= FANMINSTARTTEMP)) 
  {
    //pin_low(PN_FAN_ENA);
    if((pin_high(PN_FAN_ENA) == 1) && (pin_high(PN_HEAT_ENA) == 1)) return 1;
    else return 0;
  }
  else if((fanTempDiffHigh <= FANSTOPTEMP) && (((cal.tmax + 5) / 10) <= FANMAXSTOPTEMP))
  {
    if((pin_low(PN_FAN_ENA) == 1) && (pin_low(PN_HEAT_ENA) == 1)) return 0;
    else return 0;
  }
}


