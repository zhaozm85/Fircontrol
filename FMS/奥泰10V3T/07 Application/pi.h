/**
  ******************************************************************************
  * proj    BMU_HM
  * file    pi.h
  * ver     1.0
  * brief   产品信息
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
/* 防止头文件被重复引用 ------------------------------------------------------*/

#ifndef _PI_H_
#define _PI_H_

/* Includes ------------------------------------------------------------------*/
/* 本头文件涉及的头文件 ------------------------------------------------------*/

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
    /* 创建时间和更新时间记以2000.1.1 0:00:00开始经过的秒数 */
    volatile unsigned long  wCreateTime;  /* 创建时间 */
    volatile unsigned long  wUpdateTime;  /* 更新时间 */
    volatile unsigned short hUpdateCount; /* 更新次数 */
    volatile unsigned char  bCSEven;  /* 偶部分的校验和 */
    volatile unsigned char  bCSOdd;   /* 奇部分的校验和 */
  }sHead;
  /* 产品信息 */
  volatile unsigned char  baSN[8];  /* 序列号 */
}
gt_sPI;

/* Global consts -------------------------------------------------------------*/
/* 全局常量声明 --------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
/* 全局变量声明 --------------------------------------------------------------*/

extern gt_sPI gv_sPI;

/* Global function prototypes ------------------------------------------------*/
/* 全局函数声明 --------------------------------------------------------------*/

extern int gf_nPIInit(void);
extern int gf_nPILoad(void);
extern int gf_nPIUnlock(unsigned char key);
extern int gf_nPISetSN(unsigned char * sn);
extern int gf_nPISave(void);

/* File Id & Re-include control ----------------------------------------------*/
/* 防止头文件被重复引用 ------------------------------------------------------*/

#endif  /* _PI_H_ */

