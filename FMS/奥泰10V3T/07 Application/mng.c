/**
  ******************************************************************************
  * proj    firmware startup project
  * file    MNG.c
  * ver     1.0
  * brief   This is a MNG file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _MNG_C_  0x77
#define FILE_No _MNG_C_

/* Includes ------------------------------------------------------------------*/
#include  "mng.h"  /* mandotary */
#include  "can.h"
#include  "mcu.h"
#include  "sc.h"
#include  "pi.h"

/* Private Macros & Defines --------------------------------------------------*/

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  /* can */
  struct
  {
    can_frame_t rxm;
    unsigned char we;
    unsigned char re;
    can_frame_t txm;
  }
  can;
  /* cmd */
  struct
  {
    unsigned char grp;  //  1xx, 3xx, 5xx, 7xx
    unsigned char sub;  //  xx1, xx2, xx3, ...
    unsigned char tgt;  //  target address
    unsigned char mod;  //  module
    unsigned char cmd;  //  command
    unsigned char arg;  //  argument
    unsigned long val;  //  value
  }
  cmd;
}
mng_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

mng_t mng;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    MNG function
  * param   none
  * retval  none
  */

void mng_crx_save(can_frame_t* f)
{

  if( mng.can.we ){
    mng.can.rxm = *f;
    mng.can.re = 1;
    mng.can.we = 0;
  }

}

static void lf_vMngExec700_M0_Info(void)
{
}

static void lf_vMngExec700_M0_Oper(void)
{
}

static void lf_vMngExec700_M0_Read(void)
{
  unsigned long val;
  DBG_PRINTF(" - Exec700 M0 Read\r\n");

  /* 读取数据 */
  switch( mng.cmd.arg ){
    case  0:
      val = rtc_sec;
      break;
    default:
      val = 0xffffffff;
      break;    
  }

  /* 准备数据 */
  mng.can.txm.id = 0x700 | ((gv_sSC.sWX.bModNum &0xf) <<4);
  mng.can.txm.dlc = 8;
  mng.can.txm.data[0] = mng.cmd.tgt;
  mng.can.txm.data[1] = mng.cmd.mod;
  mng.can.txm.data[2] = mng.cmd.cmd;
  mng.can.txm.data[3] = mng.cmd.arg;
  mng.can.txm.data[4] = (unsigned char)(val>>24);
  mng.can.txm.data[5] = (unsigned char)(val>>16);
  mng.can.txm.data[6] = (unsigned char)(val>>8);
  mng.can.txm.data[7] = (unsigned char)(val>>0);

  /* 发送消息 */
  (void)can_xmit(&mng.can.txm);
}

static void lf_vMngExec700_M0_Write(void)
{
  unsigned long val;
  DBG_PRINTF(" - Exec700 M0 Write\r\n");
  
  /* 提取数据 */
  val = mng.cmd.val;
  
  /* 写入数据 */
  switch( mng.cmd.arg ){
    case  0:
      rtc_sec = val;
      break;
    default:
      val = 0xffffffff;
      break;    
  }
  
  /* 准备数据 */
  mng.can.txm.id = 0x700 | ((gv_sSC.sWX.bModNum &0xf) <<4);
  mng.can.txm.dlc = 8;
  mng.can.txm.data[0] = mng.cmd.tgt;
  mng.can.txm.data[1] = mng.cmd.mod;
  mng.can.txm.data[2] = mng.cmd.cmd;
  mng.can.txm.data[3] = mng.cmd.arg;
  mng.can.txm.data[4] = (unsigned char)(val>>24);
  mng.can.txm.data[5] = (unsigned char)(val>>16);
  mng.can.txm.data[6] = (unsigned char)(val>>8);
  mng.can.txm.data[7] = (unsigned char)(val>>0);

  /* 发送消息 */
  (void)can_xmit(&mng.can.txm);  
}

static void lf_vMngExec700_M0(void)
{
  DBG_PRINTF(" - Exec700 M0\r\n");

  switch( mng.cmd.cmd ){
    /* 基本信息 */
    case  0:  lf_vMngExec700_M0_Info();  break;
    /* 基本操作 */
    case  1:  lf_vMngExec700_M0_Oper();  break;
    /* 读取参数 */
    case  2:  lf_vMngExec700_M0_Read();  break;
    /* 写入参数 */
    case  3:  lf_vMngExec700_M0_Write(); break;
    default:  break;
  }
}

static void lf_vMngExec700_PI_Info(void)
{
  unsigned char i;
  DBG_PRINTF(" - Exec700 PI Info\r\n");

  /* 准备数据 */
  mng.can.txm.id = 0x700 | ((gv_sSC.sWX.bModNum &0xf) <<4);
  mng.can.txm.dlc = 8;
  for(i=0;i<8;i++) mng.can.txm.data[i] = gv_sPI.baSN[i];

  /* 发送消息 */
  (void)can_xmit(&mng.can.txm);
}

static void lf_vMngExec700_PI_Oper(void)
{
  int res;
  DBG_PRINTF(" - Exec700 PI Oper\r\n");

  /* 执行操作 */
  switch( mng.cmd.arg ){
    case  0: res = gf_nPIInit(); break;
    case  1: res = gf_nPILoad(); break;
    case  2: res = gf_nPISave(); break;
    case  3: res = gf_nPIUnlock((unsigned char)mng.cmd.val); break;
    default: res = NG;      
  }

  /* 准备数据 */
  mng.can.txm.id = 0x700 | ((gv_sSC.sWX.bModNum &0xf) <<4);
  mng.can.txm.dlc = 8;
  mng.can.txm.data[0] = mng.cmd.tgt;
  mng.can.txm.data[1] = mng.cmd.mod;
  mng.can.txm.data[2] = mng.cmd.cmd;
  mng.can.txm.data[3] = mng.cmd.arg;
  mng.can.txm.data[4] = (unsigned char)((long)res>>24);
  mng.can.txm.data[5] = (unsigned char)((long)res>>16);
  mng.can.txm.data[6] = (unsigned char)((long)res>>8);
  mng.can.txm.data[7] = (unsigned char)((long)res>>0);

  /* 发送消息 */
  (void)can_xmit(&mng.can.txm);
}

static void lf_vMngExec700_PI_Read(void)
{
  DBG_PRINTF(" - Exec700 PI Read\r\n");

  /* 读取数据 */
  if( mng.cmd.arg<6 ){
    unsigned char * p = (unsigned char *)&gv_sPI;
    mng.can.txm.data[4] = p[mng.cmd.arg*4+0];
    mng.can.txm.data[5] = p[mng.cmd.arg*4+1];
    mng.can.txm.data[6] = p[mng.cmd.arg*4+2];
    mng.can.txm.data[7] = p[mng.cmd.arg*4+3];
  }else{
    mng.can.txm.data[4] = 0xff;
    mng.can.txm.data[5] = 0xff;
    mng.can.txm.data[6] = 0xff;
    mng.can.txm.data[7] = 0xff;
  }

  /* 准备数据 */
  mng.can.txm.id = 0x700 | ((gv_sSC.sWX.bModNum &0xf) <<4);
  mng.can.txm.dlc = 8;
  mng.can.txm.data[0] = mng.cmd.tgt;
  mng.can.txm.data[1] = mng.cmd.mod;
  mng.can.txm.data[2] = mng.cmd.cmd;
  mng.can.txm.data[3] = mng.cmd.arg;

  /* 发送消息 */
  (void)can_xmit(&mng.can.txm);
}

static void lf_vMngExec700_PI_Write(void)
{
  DBG_PRINTF(" - Exec700 PI Read\r\n");

  /* 写入数据 */
  if( mng.cmd.arg>=4 && mng.cmd.arg<6 ){
    unsigned char * p = (unsigned char *)&gv_sPI;
    p[mng.cmd.arg*4+0] = mng.can.rxm.data[4];
    p[mng.cmd.arg*4+1] = mng.can.rxm.data[5];
    p[mng.cmd.arg*4+2] = mng.can.rxm.data[6];
    p[mng.cmd.arg*4+3] = mng.can.rxm.data[7];
  }

  /* 读回数据 */
  if( mng.cmd.arg<6 ){
    unsigned char * p = (unsigned char *)&gv_sPI;
    mng.can.txm.data[4] = p[mng.cmd.arg*4+0];
    mng.can.txm.data[5] = p[mng.cmd.arg*4+1];
    mng.can.txm.data[6] = p[mng.cmd.arg*4+2];
    mng.can.txm.data[7] = p[mng.cmd.arg*4+3];
  }else{
    mng.can.txm.data[4] = 0xff;
    mng.can.txm.data[5] = 0xff;
    mng.can.txm.data[6] = 0xff;
    mng.can.txm.data[7] = 0xff;
  }

  /* 准备数据 */
  mng.can.txm.id = 0x700 | ((gv_sSC.sWX.bModNum &0xf) <<4);
  mng.can.txm.dlc = 8;
  mng.can.txm.data[0] = mng.cmd.tgt;
  mng.can.txm.data[1] = mng.cmd.mod;
  mng.can.txm.data[2] = mng.cmd.cmd;
  mng.can.txm.data[3] = mng.cmd.arg;

  /* 发送消息 */
  (void)can_xmit(&mng.can.txm);
}

static void lf_vMngExec700_PI(void)
{
  DBG_PRINTF(" - Exec700 PI\r\n");

  switch( mng.cmd.cmd ){
    /* 基本信息 */
    case  0:  lf_vMngExec700_PI_Info();  break;
    /* 基本操作 */
    case  1:  lf_vMngExec700_PI_Oper();  break;
    /* 读取参数 */
    case  2:  lf_vMngExec700_PI_Read();  break;
    /* 写入参数 */
    case  3:  lf_vMngExec700_PI_Write(); break;
    default:  break;
  }
}

static void lf_vMngExec700_SC(void)
{
}

unsigned char mng_crx_proc(void)
{

  unsigned char exit = 0;

  /* 有数据时，开始处理 */
  if( mng.can.re ){  
    unsigned long id;
    unsigned short mngfd[6];
    unsigned char i;
    LED_BLUE = LED_ON;
    /* 只处理标准帧 */
    id = mng.can.rxm.id;
    mngfd[0] = mng.can.rxm.data[0];
    mngfd[0] |= mng.can.rxm.data[1];
    
    if (mngfd[0]>1000) {
    mng.can.txm.id = 0x012;
    mng.can.txm.dlc = 8;
    for(i=0;i<8;i++) mng.can.txm.data[i] = 1;
    (void)can_xmit(&mng.can.txm);
    LED_GREEN = LED_OFF;
    }
  
   
    mng.can.re = 0;
    mng.can.we = 1;
    LED_BLUE = LED_OFF;
  }

  return exit;
}

void mng_init(void)
{
  mng.can.re = 0;
  mng.can.we = 1;
  (void)can_init();
  dec_tick = 300; //  3 sec
}

void mng_reset(void)
{
  (void)can_reset();
}

void mng_info(void)
{
  unsigned char i;

  if( dec_tick==0 ){
    dec_tick = 300;

    //  serial number
    LED_GREEN = LED_ON;
    mng.can.txm.id = 0x500+(gv_sSC.sWX.bModNum<<4)+0;
    mng.can.txm.dlc = 8;
    for(i=0;i<8;i++) mng.can.txm.data[i] = gv_sPI.baSN[i];
    (void)can_xmit(&mng.can.txm);
    LED_GREEN = LED_OFF;

  }
}
