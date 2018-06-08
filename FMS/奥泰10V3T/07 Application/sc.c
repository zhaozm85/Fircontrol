/**
  *****************************************************************************
  * proj    BMU_HM
  * file    sc.c
  * ver     1.0
  * brief   系统配置
  -----------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id ------------------------------------------------------------------*/
/* 源文件标识 ---------------------------------------------------------------*/
#define _SC_C_  0x7A
#define FILE_No _SC_C_

/* Includes -----------------------------------------------------------------*/
/* 本源文件涉及的头文件 -----------------------------------------------------*/

#include  "sc.h"  /* 强制引用与本源文件对应的头文件 */
#include  "at24c.h"
#include  "mcu.h"
#include  "ntc.h"

/* Local Macros & Defines ---------------------------------------------------*/
/* 本地宏定义 ---------------------------------------------------------------*/

#define   SC_MAGIC_CODE         0x5500
#define   SC_ADDRESS_IN_EEPROM  0x0100
#define   SC_UPDATE_COUNT_MAX   50000

/* Local typedefs -----------------------------------------------------------*/
/* 本地类型定义 -------------------------------------------------------------*/

/* 基本类型，用于操作控制 */
typedef struct
{
  union {
    unsigned char bAll;
    struct{
      unsigned  Loaded:1;
      unsigned  Checked:1;
    }sFlag;
  }uFlag;
}
lt_sSC;

/* Local consts -------------------------------------------------------------*/
/* 本地常量定义 -------------------------------------------------------------*/

/* 产品信息的默认初值 */
const gt_sSC  lc_sSCDefault = {
  //  head
  {
    sizeof(gt_sSC),
    SC_MAGIC_CODE,
    0,
    0,
    0,
    0,
    0,
  },
  //  基本配置
  {
    MSCAN_BRP_250K,   //  can baudrate
    BMU_MODE_NORMAL,  //  default mode
  },
  //  电压采集
  {
    3,  //  ltc6802 count, if count is 3, it is SY project 
    {
      { 0x01, PN_VS_ENA_A, PN_VS_NCS_A },
      { 0x0C, PN_VS_ENA_B, PN_VS_NCS_B },
    },
  },
  //  温度采集
  {
    5,  //  adc wait msecs
    512,  //  adc repeat count
    {     //  sensor type
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
    },
    6, //  count1
    {   //  list 1
      14, 21, 7, 22, 15, 23,
      1, 10, 2, 5, 7, 11,
      12, 13, 14, 15, 3, 6,
      4, 19, 20, 21, 22, 23,
    },
    24,  // count 2
    {   // list 2
      14, 21, 7, 22, 15, 23,
      1, 10, 2, 5, 7, 11,
      12, 13, 14, 15, 3, 6,
      4, 19, 20, 21, 22, 23,
    },
    1396, //  VTemp25
    3638, //  m hot
    3266, //  m cold
  },
  //  统计计算
  {
    //  参考电压
    {
      3330, 3331, 3332, 3333,
      3334, 3335, 3336, 3337,
      3338, 3339, 3340, 3341,
    },
    //24,  //  电压个数
    24,  //  电压个数
    12,   //The cell's number of single Ltc6802 
    //  电压列表
    /*{
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b,
      0x10, 0x11, 0x12, 0x13,
      0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b,
    },*/
    {
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b,
      0x10, 0x11, 0x12, 0x13,
      0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 
    },
    //  电压校正表
    {
      0,0,0,0,0,0,
      0,0,0,0,0,0,
    },
    //  参考温度
    {
      850, 851, 852, 853, 854, 855,
      860, 861, 862, 863, 864, 865,
      870, 871, 872, 873, 874, 875,
      880, 881, 882, 883, 884, 885,
    },
    24,  //  温度个数
    12,    //The temp resistor number of single Ltc6802
    //  温度列表
    {
      14, 21, 7, 22, 15, 23,
      1, 10, 2, 5, 7, 11,
      12, 13, 14, 15, 3, 6,
      4, 19, 20, 21, 22, 23,
    },
  },
  //  均衡控制
  {
    1,  //  底层允许
    20, //  底层周期时间
    5,  //  底层开启时间
    500+400,  //  凉判断温度
    600+400,  //  热判断温度
    0,  //  外部允许
    1,  //  内部允许
    30*5, //  内部周期时间
    20*5, //  内部开启时间
    50,   //  内部均衡开启门限电压
  },
  //  万象协议控制
  {
    1,  //  组号
    1, //  模块号  bModNum
    20, //  同步周期 200ms
    10, //  同步超时因数
    24,  //  电压个数
    12,   //The cell's number of single Ltc6802
    24,  //  温度个数
    12,   //The temp resistor number of single Ltc6802
    0,    //fan Status of cell 1 to 12. Start = 1; Stop = 0.
    0,    //fan Status of cell 12 to 24. Start = 1; Stop = 0.
    //  温度列表
    {
      16, 8, 17, 0, 18, 9, 0xff, 0xff,
    },    
  },
};

/* Local variables ----------------------------------------------------------*/
/* 本地变量定义 -------------------------------------------------------------*/

/* 用于操作控制 */
lt_sSC  lv_sSC;

/* Global consts ------------------------------------------------------------*/
/* 全局常量定义 -------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/
/* 全局变量定义 -------------------------------------------------------------*/

gt_sSC  gv_sSC;

/* Local functions ----------------------------------------------------------*/
/* 本地函数实现 -------------------------------------------------------------*/

/**
  * name    lf_nSCLoad
  * bref    将产品信息从EEPROM中载入到RAM中
  * param   none
  * retval  none
  */
int lf_nSCLoad(void)
{
  int res;
  
  /* 打开EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* 读取EEPROM */
  res = at24c_read(SC_ADDRESS_IN_EEPROM,(void*)&gv_sSC,sizeof(gv_sSC));
  if( res!=OK ) return res;
  
  /* 关闭EEPROM */
  res = at24c_reset();
  if( res!=OK ) return res;
  
  /* 完成 */
  lv_sSC.uFlag.sFlag.Loaded = 1;
  return  OK;
}

/**
  * name    lf_nSCCheck
  * bref    检查RAM中的产品信息是否有效
  * param   none
  * retval  none
  */
int lf_nSCCheck(void)
{

  /* Length */
  if( gv_sSC.sHead.hLength==0 || gv_sSC.sHead.hLength==0xFFFF ){
    DBG_PRINTF(" ***NG*** Invalid sHead.hLength: %Xh\r\n", gv_sSC.sHead.hLength);
    return  NG;
  }

  /* MAGIC CODE */
  if( gv_sSC.sHead.hMagicCode!=SC_MAGIC_CODE ){
    DBG_PRINTF(" ***NG*** Invalid Magic Code : %Xh\r\n", gv_sSC.sHead.hMagicCode);
    return NG;
  }

  /* CSEven, CSOdd */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sSC;
    /* CSEven */
    for(cs=0,i=0;i<gv_sSC.sHead.hLength;i+=2){      
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSEven failed: %Xh\r\n", cs);
      return  NG;
    }
    /* CSOdd */
    for(cs=0,i=1;i<gv_sSC.sHead.hLength;i+=2){
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSOdd failed: %Xh\r\n", cs);
      return  NG;
    }
  }

  /* 通过 */
  lv_sSC.uFlag.sFlag.Checked = 1;
  return  OK;
}

/**
  * name    lf_nSCUpdate
  * bref    更新产品信息的时间和次数
  * param   none
  * retval  none
  */
static int lf_nSCUpdate(void)
{
  /* 更新时间 */
  DisableInterrupts;
  gv_sSC.sHead.wUpdateTime = rtc_sec;
  EnableInterrupts;
  
  /* 更新次数 */
  if( gv_sSC.sHead.hUpdateCount>=SC_UPDATE_COUNT_MAX ){
    DBG_PRINTF(" ***NG*** Update Count Outbreak Limit\r\n");
    return  NG;
  }
  gv_sSC.sHead.hUpdateCount ++;
  
  /* 创建时间 */
  if( gv_sSC.sHead.hUpdateCount==1 ){
    gv_sSC.sHead.wCreateTime = gv_sSC.sHead.wUpdateTime;
  }

  /* CS */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sSC;
    gv_sSC.sHead.bCSEven = 0;
    gv_sSC.sHead.bCSOdd = 0;
    /* CSEven */
    for(cs=0,i=0;i<gv_sSC.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sSC.sHead.bCSEven = cs;
    DBG_PRINTF(" - CSEven: %Xh\r\n", cs);
    /* CSOdd */
    for(cs=0,i=1;i<gv_sSC.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sSC.sHead.bCSOdd = cs;
    DBG_PRINTF(" - CSOdd: %Xh\r\n", cs);
  }

  /* 完成 */
  return  OK;
}

/**
  * name    lf_nSCSave
  * bref    将RAM中的产品信息保存到EEPROM中
  * param   none
  * retval  none
  */
static int lf_nSCSave(void)
{
  int res;
  
  /* 打开EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* 写入EEPROM */
  res = at24c_write(SC_ADDRESS_IN_EEPROM,(void*)&gv_sSC,sizeof(gv_sSC));
  if( res!=OK ) return res;
  
  /* 关闭EEPROM */
  res = at24c_reset();
  if( res!=OK ) return res;
  
  /* 完成 */
  return  OK;
}

/* Global functions ---------------------------------------------------------*/
/* 全局函数实现 -------------------------------------------------------------*/

/**
  * name    gf_nSCInit
  * bref    初始化
  * param   none
  * retval  none
  */
int gf_nSCInit(void)
{
  DBG_PRINTF("gf_nSCInit()\r\n");

  /* 初始化 */
  lv_sSC.uFlag.bAll = 0;
  /* 完成 */
  return  OK;
}

/**
  * name    gf_nSCLoad
  * bref    将产品信息从EEPROM中载入到RAM中
  * param   none
  * retval  none
  */
int gf_nSCLoad(void)
{
#if 0
  int res;
  DBG_PRINTF("gf_nSCLoad()\r\n");
  
  /* 从EEPROM中载入产品信息 */
  res = lf_nSCLoad(); if( res!=OK ) return res;

  /* 如果EEPROM中没有产品信息，则需要初始化 */
  if( lf_nSCCheck()==NG ){  
    /* 设置初始值 */
    gv_sSC = lc_sSCDefault;
    /* 更新 */
    res = lf_nSCUpdate(); if( res!=OK ) return res;
    /* 保存到EEPROM */
    res = lf_nSCSave(); if( res!=OK ) return res;
    /* 完成 */
    DBG_PRINTF(" - OK: SC Set Default\r\n");
    /* 利用看门狗重启 */    
    for(;;);
  }
#else
  gv_sSC = lc_sSCDefault;
#endif

  /* 完成 */
  DBG_PRINTF(" - OK: SC Loaded: %d\r\n", gv_sSC.sHead.hUpdateCount);
  return  OK;
}

/**
  * name    gf_nSCSave
  * bref    将产品信息保存到EEPROM中
  * param   none
  * retval  none
  */
int gf_nSCSave(void) {

  int res;
  DBG_PRINTF("gf_nSCSave()\r\n");

  /* 未载入时不能操作 */
  if( lv_sSC.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** SC Not Loaded\r\n");
    return NG;
  }
  
  /* 检验未通过不能操作 */
  if( lv_sSC.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** SC Not Checked\r\n");
    return  NG;
  }
  
  /* 更新产品信息 */
  res = lf_nSCUpdate();
  if( res!=OK ) return res;
  
  /* 保存到EEPROM */
  res = lf_nSCSave();
  if( res!=OK ) return res;
  
  /* 完成 */
  DBG_PRINTF(" - OK: SC Save\r\n");
  return  OK;
}

