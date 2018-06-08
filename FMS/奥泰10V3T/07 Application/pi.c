/**
  *****************************************************************************
  * proj    BMU_HM
  * file    pi.c
  * ver     1.0
  * brief   产品信息
  -----------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id ------------------------------------------------------------------*/
/* 源文件标识 ---------------------------------------------------------------*/
#define _PI_C_  0x79
#define FILE_No _PI_C_

/* Includes -----------------------------------------------------------------*/
/* 本源文件涉及的头文件 -----------------------------------------------------*/

#include  "pi.h"  /* 强制引用与本源文件对应的头文件 */
#include  "at24c.h"
#include  "mcu.h"

/* Local Macros & Defines ---------------------------------------------------*/
/* 本地宏定义 ---------------------------------------------------------------*/

#define   PI_MAGIC_CODE         0x55AA
#define   PI_ADDRESS_IN_EEPROM  0x0000
#define   PI_UPDATE_COUNT_MAX   50000

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
      unsigned  Unlocked:1;
    }sFlag;
  }uFlag;
}
lt_sPI;

/* Local consts -------------------------------------------------------------*/
/* 本地常量定义 -------------------------------------------------------------*/

/* 产品信息的默认初值 */
const gt_sPI  lc_sPIDefault = {
  {
    sizeof(gt_sPI),
    PI_MAGIC_CODE,
    0,
    0,
    0,
    0,
    0,
  },
  { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF  } // 产品序列号
};

/* Local variables ----------------------------------------------------------*/
/* 本地变量定义 -------------------------------------------------------------*/

/* 用于操作控制 */
lt_sPI  lv_sPI;

/* Global consts ------------------------------------------------------------*/
/* 全局常量定义 -------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/
/* 全局变量定义 -------------------------------------------------------------*/

gt_sPI  gv_sPI;

/* Local functions ----------------------------------------------------------*/
/* 本地函数实现 -------------------------------------------------------------*/

/**
  * name    lf_nPILoad
  * bref    将产品信息从EEPROM中载入到RAM中
  * param   none
  * retval  none
  */
static int lf_nPILoad(void)
{
#if 0  
  int res;

  /* 打开EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* 读取EEPROM */
  res = at24c_read(PI_ADDRESS_IN_EEPROM,(void*)&gv_sPI,sizeof(gv_sPI));
  if( res!=OK ) return res;
  
  /* 关闭EEPROM */
  res = at24c_reset();
  if( res!=OK ) return res;
#else
  gv_sPI = lc_sPIDefault;
#endif
  
  /* 完成 */
  lv_sPI.uFlag.sFlag.Loaded = 1;
  return  OK;
}

/**
  * name    lf_nPICheck
  * bref    检查RAM中的产品信息是否有效
  * param   none
  * retval  none
  */
int lf_nPICheck(void)
{

  /* Length */
  if( gv_sPI.sHead.hLength==0 || gv_sPI.sHead.hLength==0xFFFF ){
    DBG_PRINTF(" ***NG*** Invalid sHead.hLength: %Xh\r\n", gv_sPI.sHead.hLength);
    return  NG;
  }

  /* MAGIC CODE */
  if( gv_sPI.sHead.hMagicCode!=PI_MAGIC_CODE ){
    DBG_PRINTF(" ***NG*** Invalid Magic Code : %Xh\r\n", gv_sPI.sHead.hMagicCode);
    return NG;
  }

  /* CSEven, CSOdd */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sPI;
    /* CSEven */
    for(cs=0,i=0;i<gv_sPI.sHead.hLength;i+=2){
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSEven failed: %Xh\r\n", cs);
      return  NG;
    }
    /* CSOdd */
    for(cs=0,i=1;i<gv_sPI.sHead.hLength;i+=2){
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSOdd failed: %Xh\r\n", cs);
      return  NG;
    }
  }

  /* 通过 */
  lv_sPI.uFlag.sFlag.Checked = 1;
  return  OK;
}

/**
  * name    lf_nPIUnlock
  * bref    匹配解锁，允许修改
  * param   none
  * retval  none
  */
static int lf_nPIUnlock(unsigned char key)
{
  int i;

  /* 匹配 */
  for(i=0;i<8;i++) key += gv_sPI.baSN[i];
  if( key!=0 ){
    DBG_PRINTF(" ***NG*** Unmatched Key: %Xh\r\n", key);
    return NG;
  }

  /* 通过 */
  lv_sPI.uFlag.sFlag.Unlocked = 1;
  return  OK;
}

/**
  * name    lf_nPIUpdate
  * bref    更新产品信息的时间和次数
  * param   none
  * retval  none
  */
static int lf_nPIUpdate(void)
{
  /* 更新时间 */
  DisableInterrupts;
  gv_sPI.sHead.wUpdateTime = rtc_sec;
  EnableInterrupts;
  
  /* 更新次数 */
  if( gv_sPI.sHead.hUpdateCount>=PI_UPDATE_COUNT_MAX ){
    DBG_PRINTF(" ***NG*** Update Count Outbreak Limit\r\n");
    return  NG;
  }
  gv_sPI.sHead.hUpdateCount ++;
  
  /* 创建时间 */
  if( gv_sPI.sHead.hUpdateCount==1 ){
    gv_sPI.sHead.wCreateTime = gv_sPI.sHead.wUpdateTime;
  }

  /* CS */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sPI;
    gv_sPI.sHead.bCSEven = 0;
    gv_sPI.sHead.bCSOdd = 0;
    /* CSEven */
    for(cs=0,i=0;i<gv_sPI.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sPI.sHead.bCSEven = cs;
    DBG_PRINTF(" - CSEven: %Xh\r\n", cs);
    /* CSOdd */
    for(cs=0,i=1;i<gv_sPI.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sPI.sHead.bCSOdd = cs;
    DBG_PRINTF(" - CSOdd: %Xh\r\n", cs);
  }

  /* 完成 */
  return  OK;
}

/**
  * name    lf_nPISave
  * bref    将RAM中的产品信息保存到EEPROM中
  * param   none
  * retval  none
  */
static int lf_nPISave(void)
{
  int res;
  
  /* 打开EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* 写入EEPROM */
  res = at24c_write(PI_ADDRESS_IN_EEPROM,(void*)&gv_sPI,sizeof(gv_sPI));
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
  * name    gf_nPIInit
  * bref    初始化
  * param   none
  * retval  none
  */
int gf_nPIInit(void)
{
  DBG_PRINTF("gf_nPIInit()\r\n");

  /* 初始化 */
  lv_sPI.uFlag.bAll = 0;
  /* 完成 */
  return  OK;
}

/**
  * name    gf_nPILoad
  * bref    将产品信息从EEPROM中载入到RAM中
  * param   none
  * retval  none
  */
int gf_nPILoad(void)
{
  int res;
  DBG_PRINTF("gf_nPILoad()\r\n");
  
  /* 从EEPROM中载入产品信息 */
  res = lf_nPILoad(); if( res!=OK ) return res;
#if 0
  /* 如果EEPROM中没有产品信息，则需要初始化 */
  if( lf_nPICheck()==NG ){  
    /* 设置初始值 */
    gv_sPI = lc_sPIDefault;
    /* 更新 */
    res = lf_nPIUpdate(); if( res!=OK ) return res;
    /* 保存到EEPROM */
    res = lf_nPISave(); if( res!=OK ) return res;
    /* 完成 */
    DBG_PRINTF(" - OK: PI Set Default\r\n");
    /* 利用看门狗重启 */    
    for(;;);
  }

  /* 如果SN是全0xFF，则自动解锁 */
  {
    unsigned char i;
    for(i=0;i<8;i++){
      if( gv_sPI.baSN[i]==0xFF ) continue;
      break;
    }
    if(i==8 ){
      lv_sPI.uFlag.sFlag.Unlocked = 1;
      DBG_PRINTF(" - PI Auto Unlocked\r\n");
    }
  }
#endif
  /* 完成 */
  DBG_PRINTF(" - OK: PI Loaded: %d\r\n", gv_sPI.sHead.hUpdateCount);
  return  OK;
}

/**
  * name    gf_nPIUnlock
  * bref    将产品信息解锁
  * param   none
  * retval  none
  */
int gf_nPIUnlock(unsigned char key)
{
  int res;
  DBG_PRINTF("gf_nPIUnlock(%Xh)\r\n", key);

  /* 未载入时不能操作 */
  if( lv_sPI.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** PI Not Loaded\r\n");
    return NG;
  }
  
  /* 检验未通过不能操作 */
  if( lv_sPI.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Checked\r\n");
    return  NG;
  }
  
  /* 解锁 */
  res = lf_nPIUnlock(key);
  if( res!=OK ) return res;
  
  /* 完成 */
  DBG_PRINTF(" - OK: PI Unlocked\r\n");
  return  OK;
}

/**
  * name    gf_nPISetSN
  * bref    设置产品信息中的序列号
  * param   none
  * retval  none
  */
int gf_nPISetSN(unsigned char * sn)
{ 
  unsigned char i;
  DBG_PRINTF("gf_nPISetSN(sn)\r\n");

  /* 未载入时不能操作 */
  if( lv_sPI.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** PI Not Loaded\r\n");
    return NG;
  }
  
  /* 检验未通过不能操作 */
  if( lv_sPI.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Checked\r\n");
    return  NG;
  }

  /* 未解锁不能操作 */
  if( lv_sPI.uFlag.sFlag.Unlocked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Unlocked\r\n");
    return  NG;
  }
  
  /* 设置 */
  for(i=0;i<8;i++) gv_sPI.baSN[i] = sn[i];
  
  /* 完成 */
  DBG_PRINTF(" - OK: PI Set SN\r\n");
  return  OK;
}

/**
  * name    gf_nPISave
  * bref    将产品信息保存到EEPROM中
  * param   none
  * retval  none
  */
int gf_nPISave(void) {

  int res;
  DBG_PRINTF("gf_nPISave()\r\n");

  /* 未载入时不能操作 */
  if( lv_sPI.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** PI Not Loaded\r\n");
    return NG;
  }
  
  /* 检验未通过不能操作 */
  if( lv_sPI.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Checked\r\n");
    return  NG;
  }

  /* 未解锁不能操作 */
  if( lv_sPI.uFlag.sFlag.Unlocked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Unlocked\r\n");
    return  NG;
  }
  
  /* 更新产品信息 */
  res = lf_nPIUpdate();
  if( res!=OK ) return res;
  
  /* 保存到EEPROM */
  res = lf_nPISave();
  if( res!=OK ) return res;
  
  /* 完成 */
  DBG_PRINTF(" - OK: PI Save\r\n");
  return  OK;
}

